/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 * -----------------------------------------------------------------------------
 * Thread "self-awareness"
 *
 * The Self module implements TLS management with the memory from mempool.h and
 * guards further modules from receiving events before threads are
 * initialized or after they are finished.
 *
 * The Self module subscribes to INTERCEPT_{BEFORE,AFTER,EVENT} chains and
 * interrupts them, ie, any module subscribing after Self module does not
 * receive events in the INTERCEPT chains. The Self module republishes all
 * events in equivalent CAPTURE chains, but these contain TLS metadata object
 * can can be used by the user.
 *
 * When a self object is created, an EVENT_SELF_INIT is published in
 * CAPTURE_EVENT. When a self object is deleted, an EVENT_SELF_FINI is
 * published.
 *
 * Self subscribes THREAD_JOIN events to immediately delete self objects. For
 * other threads (eg, detached threads), Self eventually garbage collect the
 * self object.
 */
#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#ifndef DICE_MODULE_PRIO
    #define DICE_MODULE_PRIO 4
#endif
#include <dice/chains/intercept.h>
#include <dice/events/pthread.h>
#include <dice/log.h>
#include <dice/mempool.h>
#include <dice/module.h>
#include <dice/pubsub.h>
#include <dice/rbtree.h>
#include <dice/self.h>
#include <dice/thread_id.h>
#include <vsync/atomic.h>
#include <vsync/spinlock/caslock.h>
#include <vsync/stack/quack.h>

/* self object represents each thread in the CAPTURE chains. */
struct self {
    metadata_t md;
    struct rbnode map_node;
    struct quack_node_s retired_node;
    pthread_t pid;
    thread_id tid;
    struct rbtree tls;
    uint64_t count;
    int guard;
    bool retired;
};

/* tls_item is a memory object allocated in the thread-local storage (tls) */
struct tls_item {
    uintptr_t key;
    struct rbnode node;
    char data[];
};

// -----------------------------------------------------------------------------
// tls items
// -----------------------------------------------------------------------------

DICE_HIDE int
_tls_cmp(const struct rbnode *a, const struct rbnode *b)
{
    const struct tls_item *ea = container_of(a, struct tls_item, node);
    const struct tls_item *eb = container_of(b, struct tls_item, node);
    return (ea->key > eb->key);
}

DICE_HIDE void
_tls_init(struct self *self)
{
    rbtree_init(&self->tls, _tls_cmp);
}

DICE_HIDE void
_tls_fini(struct self *self)
{
    // TODO: iterate over all items and mempool_free them
    (void)self;
}

// -----------------------------------------------------------------------------
// public interface
// -----------------------------------------------------------------------------

DICE_HIDE_IF thread_id
self_id(metadata_t *md)
{
    struct self *self = (struct self *)md;
    return self ? self->tid : NO_THREAD;
}

DICE_HIDE_IF void *
self_tls(metadata_t *md, const void *global, size_t size)
{
    uintptr_t item_key = (uintptr_t)global;
    struct self *self  = (struct self *)md;

    // should never be called before the self initialization
    assert(self != NULL);

    struct tls_item key = {.key = item_key};
    struct rbnode *item = rbtree_find(&self->tls, &key.node);

    if (item) {
        struct tls_item *i = container_of(item, struct tls_item, node);
        return i->data;
    }

    struct tls_item *i = mempool_alloc(size + sizeof(struct tls_item));
    if (i == NULL)
        return NULL;
    memset(&i->data, 0, size);
    i->key = item_key;
    rbtree_insert(&self->tls, &i->node);
    return i->data;
}

// -----------------------------------------------------------------------------
// thread map and cache using pthread_get/setspecific
// -----------------------------------------------------------------------------

static struct {
    caslock_t lock;
    struct rbtree map;
    vatomic64_t count;
    pthread_key_t cache_key;
    quack_t retired;
} _threads;

static void
_thread_cache_destruct(void *arg)
{
    (void)arg;
}

static void
_thread_cache_init(void)
{
    if (pthread_key_create(&_threads.cache_key, _thread_cache_destruct) != 0)
        abort();
}

static void
_thread_cache_set(struct self *self)
{
    if (pthread_setspecific(_threads.cache_key, self) != 0)
        log_fatal("could not set key");
}

DICE_HIDE struct self *
_thread_cache_get(void)
{
    return (struct self *)pthread_getspecific(_threads.cache_key);
}

static void
_thread_cache_del(void)
{
    if (pthread_setspecific(_threads.cache_key, 0) != 0)
        abort();
}

DICE_HIDE struct self *
_create_self()
{
    struct self *self;
    self        = mempool_alloc(sizeof(struct self));
    self->guard = 0;
    self->count = 0;
    self->tid   = vatomic64_inc_get(&_threads.count);
    self->pid   = pthread_self();
    _tls_init(self);
    return self;
}

static void
_destroy_self(struct self *self)
{
    _tls_fini(self);
    mempool_free(self);
}


DICE_HIDE int
_self_cmp(const struct rbnode *a, const struct rbnode *b)
{
    const struct self *ea = container_of(a, struct self, map_node);
    const struct self *eb = container_of(b, struct self, map_node);
    if ((uint64_t)ea->pid == (uint64_t)eb->pid)
        return 0;
    return ((uint64_t)ea->pid <= (uint64_t)eb->pid);
}

static void
_init_threads(void)
{
    _thread_cache_init();
    rbtree_init(&_threads.map, _self_cmp);
    caslock_init(&_threads.lock);
    quack_init(&_threads.retired);
}

static struct self *
_thread_map_find(pthread_t pid)
{
    struct self key = {.pid = pid};

    caslock_acquire(&_threads.lock);
    struct rbnode *item = rbtree_find(&_threads.map, &key.map_node);
    caslock_release(&_threads.lock);

    struct self *self = item ? container_of(item, struct self, map_node) : NULL;
    return self;
}

static void
_thread_map_put(struct self *self)
{
    assert(self);

    caslock_acquire(&_threads.lock);
    rbtree_insert(&_threads.map, &self->map_node);
    caslock_release(&_threads.lock);
}
static void
_thread_map_del(struct self *self)
{
    assert(self);
    caslock_acquire(&_threads.lock);
    rbtree_remove(&_threads.map, &self->map_node);
    caslock_release(&_threads.lock);
}

static struct self *
_get_self(void)
{
    struct self *self = _thread_cache_get();
    return self ? self : _thread_map_find(pthread_self());
}

static void
_retire_self(struct self *self)
{
    assert(self);
    assert(!self->retired);
    self->retired = true;
    _thread_cache_del();
    quack_push(&_threads.retired, &self->retired_node);
}

static void _cleanup_threads(void);

// -----------------------------------------------------------------------------
//  pubsub handler
// -----------------------------------------------------------------------------

#define self_guard(chain, type, event, self)                                   \
    do {                                                                       \
        self->guard++;                                                         \
        self->count++;                                                         \
        self->md = (metadata_t){0};                                            \
        log_debug(">> [%lu:0x%lx] %u_%u: %d\n", self_id(&self->md),            \
                  (uint64_t)pthread_self(), chain, type, self->guard);         \
        PS_PUBLISH(chain, type, event, &self->md);                             \
        log_debug("<< [%lu:0x%lx] %u_%u: %d\n", self_id(&self->md),            \
                  (uint64_t)pthread_self(), chain, type, self->guard);         \
        self->guard--;                                                         \
    } while (0)

static struct self *
_get_or_create_self(bool publish)
{
    struct self *self = _get_self();
    if (likely(self)) {
        return self;
    }

    self = _create_self();

    _thread_cache_set(self);
    _thread_map_put(self);

    if (publish)
        self_guard(CAPTURE_EVENT, EVENT_SELF_INIT, 0, self);
    return self;
}

DICE_HIDE enum ps_err
_self_handle_before(const chain_id chain, const type_id type, void *event,
                    metadata_t *md)
{
    (void)chain;
    (void)md;
    struct self *self = _get_or_create_self(true);
    assert(self);

    if (likely(self->guard++ == 0))
        self_guard(CAPTURE_BEFORE, type, event, self);
    else {
        log_printf(">>> [%lu:0x%lx] %u_%u: %d\n", self_id(&self->md),
                   (uint64_t)pthread_self(), chain, type, self->guard);
    }

    return PS_STOP_CHAIN;
}

DICE_HIDE enum ps_err
_self_handle_after(const chain_id chain, const type_id type, void *event,
                   metadata_t *md)
{
    (void)chain;
    (void)md;
    struct self *self = _get_or_create_self(true);
    assert(self);

    if (likely(self->guard-- == 1)) {
        self_guard(CAPTURE_AFTER, type, event, self);
        if (self->tid == 1)
            _cleanup_threads();
    } else {
        log_debug("<<< [%lu:0x%lx] %u_%u: %d\n", self_id(&self->md),
                  (uint64_t)pthread_self(), chain, type, self->guard);
    }

    return PS_STOP_CHAIN;
}

DICE_HIDE enum ps_err
_self_handle_event(const chain_id chain, const type_id type, void *event,
                   metadata_t *md)
{
    (void)chain;
    (void)md;
    struct self *self = _get_or_create_self(true);
    assert(self);

    if (likely(self->guard == 0))
        self_guard(CAPTURE_EVENT, type, event, self);
    else {
        log_debug("!!! [%lu:0x%lx] %u_%u: %d\n", self_id(&self->md),
                  (uint64_t)pthread_self(), chain, type, self->guard);
    }

    return PS_STOP_CHAIN;
}

PS_SUBSCRIBE(INTERCEPT_EVENT, ANY_TYPE,
             { return _self_handle_event(chain, type, event, md); })
PS_SUBSCRIBE(INTERCEPT_BEFORE, ANY_TYPE,
             { return _self_handle_before(chain, type, event, md); })
PS_SUBSCRIBE(INTERCEPT_AFTER, ANY_TYPE,
             { return _self_handle_after(chain, type, event, md); })

// -----------------------------------------------------------------------------
// init, fini and registration
// -----------------------------------------------------------------------------

static void
_cleanup_threads(void)
{
    struct quack_node_s *item = quack_popall(&_threads.retired);
    struct quack_node_s *next = NULL;

    for (; item; item = next) {
        next = item->next;

        struct self *self = container_of(item, struct self, retired_node);

        // pthread_kill with signal 0 does not do anything with the thread, but
        // if an error indicates the thread does not exist.
        if (pthread_kill(self->pid, 0) != 0) {
            self_guard(CAPTURE_EVENT, EVENT_SELF_FINI, 0, self);
            _thread_map_del(self);
            _destroy_self(self);
        } else
            quack_push(&_threads.retired, item);
    }
}

static void
_cleanup_thread(pthread_t pid)
{
    struct quack_node_s *item = quack_popall(&_threads.retired);
    struct quack_node_s *next = NULL;

    for (; item; item = next) {
        next = item->next;

        struct self *self = container_of(item, struct self, retired_node);

        if ((uint64_t)self->pid != (uint64_t)pid)
            quack_push(&_threads.retired, item);
        else {
            self_guard(CAPTURE_EVENT, EVENT_SELF_FINI, 0, self);
            _thread_map_del(self);
            _destroy_self(self);
        };
    }
}

PS_SUBSCRIBE(INTERCEPT_EVENT, EVENT_THREAD_EXIT, {
    struct self *self = _get_or_create_self(true);
    _retire_self(self);
    self_guard(CAPTURE_EVENT, EVENT_THREAD_EXIT, event, self);
    return PS_STOP_CHAIN;
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_THREAD_JOIN, {
    struct self *self             = _get_or_create_self(true);
    struct pthread_join_event *ev = EVENT_PAYLOAD(ev);
    self_guard(CAPTURE_AFTER, EVENT_THREAD_JOIN, event, self);
    _cleanup_thread(ev->thread);
})

DICE_MODULE_INIT({ _init_threads(); })

PS_SUBSCRIBE(CHAIN_CONTROL, EVENT_DICE_READY, {
    // Initialization is done, we can announce the self object of the main
    // thread was created
    (void)_get_or_create_self(true);
})

DICE_MODULE_FINI({ _cleanup_threads(); })
