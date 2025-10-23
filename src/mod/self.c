/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
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
#if defined(__NetBSD__)
    #include <errno.h>
#endif

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
    struct quack_node_s retired_node;
    uint64_t oid;
    pthread_t pid;
    thread_id tid;
    struct rbtree tls;
    int guard;
    bool retired;
};

/* tls_item is a memory object allocated in the thread-local storage (tls) */
struct tls_item {
    uintptr_t key;
    struct rbnode node;
    struct tls_dtor dtor;
    void *ptr;
};

static void cleanup_threads_(pthread_t id);

// -----------------------------------------------------------------------------
// tls items
// -----------------------------------------------------------------------------

DICE_HIDE int
tls_cmp_(const struct rbnode *a, const struct rbnode *b)
{
    const struct tls_item *ea = container_of(a, struct tls_item, node);
    const struct tls_item *eb = container_of(b, struct tls_item, node);
    return ea->key > eb->key ? 1 : ea->key < eb->key ? -1 : 0;
}

DICE_HIDE void
tls_init_(struct self *self)
{
    rbtree_init(&self->tls, tls_cmp_);
}

DICE_HIDE void
tls_fini_(struct self *self)
{
    while (self->tls.root) {
        struct rbnode *node = self->tls.root;
        rbtree_remove(&self->tls, node);

        struct tls_item *item = container_of(node, struct tls_item, node);
        assert(item->ptr);
        if (item->dtor.free)
            item->dtor.free(item->dtor.arg, item->ptr);
        mempool_free(item);
    }
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

DICE_HIDE_IF bool
self_retired(metadata_t *md)
{
    struct self *self = (struct self *)md;
    return self ? self->retired : false;
}

DICE_HIDE_IF void *
self_tls_get(metadata_t *md, uintptr_t item_key)
{
    struct self *self = (struct self *)md;

    // should never be called before the self initialization
    assert(self != NULL);

    struct tls_item key = {.key = item_key};
    struct rbnode *item = rbtree_find(&self->tls, &key.node);

    if (!item)
        return NULL;

    struct tls_item *i = container_of(item, struct tls_item, node);
    return i->ptr;
}

DICE_HIDE_IF void
self_tls_set(metadata_t *md, uintptr_t item_key, void *ptr,
             struct tls_dtor dtor)
{
    struct self *self = (struct self *)md;

    // should never be called before the self initialization
    assert(self != NULL);

    struct tls_item key = {.key = item_key};
    struct rbnode *item = rbtree_find(&self->tls, &key.node);
    struct tls_item *i  = NULL;

    if (ptr == NULL && item == NULL)
        return; // nothing to do

    if (item) {
        i = container_of(item, struct tls_item, node);
        if (i->dtor.free)
            i->dtor.free(i->dtor.arg, i->ptr);

        // remove node if ptr == NULL
        if (ptr == NULL) {
            rbtree_remove(&self->tls, &key.node);
            mempool_free(i);
            return;
        }

        // we can reuse the same node (already in the tree)
        i->ptr  = ptr;
        i->dtor = dtor;
    } else {
        i = mempool_alloc(sizeof(struct tls_item));
        if (i == NULL)
            log_fatal("mempool out of memory");

        memset(i, 0, sizeof(struct tls_item));
        i->key  = item_key;
        i->ptr  = ptr;
        i->dtor = dtor;
        rbtree_insert(&self->tls, &i->node);
    }
}

static void
mempool_free_dtor_(void *arg, void *ptr)
{
    (void)arg;
    mempool_free(ptr);
}

DICE_HIDE_IF void *
self_tls(metadata_t *md, const void *global, size_t size)
{
    uintptr_t item_key = (uintptr_t)global;

    void *data = self_tls_get(md, item_key);
    if (data)
        return data;

    void *ptr = mempool_alloc(size);
    if (ptr == NULL)
        log_fatal("mempool out of memory");
    memset(ptr, 0, size);

    self_tls_set(md, item_key, ptr,
                 (struct tls_dtor){
                     .free = mempool_free_dtor_,
                     .arg  = NULL,
                 });

    return ptr;
}

// -----------------------------------------------------------------------------
// thread cache using pthread_get/setspecific
// -----------------------------------------------------------------------------

static struct {
    caslock_t lock;
    vatomic64_t count;
    pthread_key_t cache_key;
    quack_t retired;
} threads_;

static void
thread_cache_destruct_(void *arg)
{
    (void)arg;
}

static void
thread_cache_init_(void)
{
    if (pthread_key_create(&threads_.cache_key, thread_cache_destruct_) != 0)
        abort();
}

static void
thread_cache_del_(struct self *self)
{
    (void)self;
    if (pthread_setspecific(threads_.cache_key, NULL) != 0)
        log_fatal("could not del key");
}

static void
thread_cache_set_(struct self *self)
{
    if (pthread_setspecific(threads_.cache_key, self) != 0)
        log_fatal("could not set key");
}

DICE_HIDE struct self *
thread_cache_get_(void)
{
    return (struct self *)pthread_getspecific(threads_.cache_key);
}

#if defined(__linux__)

static uint64_t
thread_oid_(void)
{
    return (uint64_t)gettid();
}
static bool
thread_dead_(struct self *self)
{
    return kill((pid_t)self->oid, 0) != 0;
}

#else  // !linux

static uint64_t
thread_oid_(void)
{
    return 0;
}

static bool
thread_dead_(struct self *self)
{
    // pthread_kill with signal 0 does not do anything with the thread, but
    // if an ESRCH error indicates the thread does not exist. See:
    // https://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread_kill.html
    //
    // This approach to figure out whether are thread is "gone" seems to
    // work most of the time. However, it is not perfect. It could give
    // false positives if the thread ID is reused by a new thread.
    //
    // In Linux, it causes the application to crash with segmentation fault.
    // So there we use the gettid and kill.
    return pthread_kill(self->pid, 0) == ESRCH;
}
#endif // !linux

DICE_HIDE struct self *
create_self_()
{
    struct self *self;
    self          = mempool_alloc(sizeof(struct self));
    self->guard   = 0;
    self->tid     = vatomic64_inc_get(&threads_.count);
    self->pid     = pthread_self();
    self->oid     = thread_oid_();
    self->retired = false;
    tls_init_(self);
    return self;
}

static void
destroy_self_(struct self *self)
{
    tls_fini_(self);

    // The main thread might still intercept after the module has terminated.
    // So, we keep the self object allocated to avoid issues.
    if (self->tid != MAIN_THREAD)
        mempool_free(self);
}


static void
init_threads_(void)
{
    thread_cache_init_();
    caslock_init(&threads_.lock);
    quack_init(&threads_.retired);
}

static struct self *
get_self_(void)
{
    // When a thread is created, it won't find its self object anywhere and
    // this function will return NULL. A new self object will be created and
    // stored in the thread specific area (here called cache).

    // When a thread is retired -- ie, between  THREAD_EXIT and SELF_FINI --
    // its self object is put into the retired stack. The object is still
    // kept in the thread_cache.

    // So when getting self an existing self, we first look in the cache. If
    // the pthread implementation does not zero the thread specific area on
    // exit, the thread will find its object there until the end of the
    // thread's lifetime. However, if the pthread implementation zeros the
    // thread specific area on exit, we need to look for the thread in the
    // retired stack.

    pthread_t pid     = pthread_self();
    struct self *self = thread_cache_get_();
    if (self)
        return self;

    // We now search the retired stack. To avoid not seeing our self object
    // while other threads are searching theirs, we have to ensure mutual
    // exlusion here.
    caslock_acquire(&threads_.lock);
    struct quack_node_s *item = quack_popall(&threads_.retired);
    struct quack_node_s *next = NULL;

    for (; item; item = next) {
        next = item->next;

        struct self *it = container_of(item, struct self, retired_node);

        if (self == NULL && it->pid == pid)
            self = it;
        quack_push(&threads_.retired, item);
    }
    caslock_release(&threads_.lock);
    assert(self == NULL || self->retired);
    return self;
}

static void
retire_self_(struct self *self)
{
    assert(self);
    assert(!self->retired);
    self->retired = true;
    thread_cache_del_(self);
    quack_push(&threads_.retired, &self->retired_node);
}

// -----------------------------------------------------------------------------
//  pubsub handler
// -----------------------------------------------------------------------------

#define self_guard(chain, type, event, self)                                   \
    do {                                                                       \
        self->guard++;                                                         \
        self->md = (metadata_t){0};                                            \
        log_debug(">> [%lu:0x%lx:%lu] %u_%u: %d", self_id(&self->md),          \
                  (uint64_t)self->pid, self->oid, chain, type, self->guard);   \
        PS_PUBLISH(chain, type, event, &self->md);                             \
        log_debug("<< [%lu:0x%lx:%lu] %u_%u: %d", self_id(&self->md),          \
                  (uint64_t)self->pid, self->oid, chain, type, self->guard);   \
        self->guard--;                                                         \
    } while (0)

DICE_HIDE enum ps_err
self_handle_before_(const chain_id chain, const type_id type, void *event,
                    struct self *self)
{
    (void)chain;
    assert(self);

    if (likely(self->guard++ == 0))
        self_guard(CAPTURE_BEFORE, type, event, self);
    else
        log_debug(">>> [%lu:0x%lx:%lu] %u_%u: %d", self_id(&self->md),
                  (uint64_t)self->pid, self->oid, chain, type, self->guard);

    assert(self->guard >= 0);
    return PS_STOP_CHAIN;
}

DICE_HIDE enum ps_err
self_handle_after_(const chain_id chain, const type_id type, void *event,
                   struct self *self)
{
    (void)chain;
    assert(self);

    if (likely(self->guard-- == 1))
        self_guard(CAPTURE_AFTER, type, event, self);
    else
        log_debug("<<< [%lu:0x%lx:%lu] %u_%u: %d", self_id(&self->md),
                  (uint64_t)self->pid, self->oid, chain, type, self->guard);

    assert(self->guard >= 0);
    return PS_STOP_CHAIN;
}

DICE_HIDE enum ps_err
self_handle_event_(const chain_id chain, const type_id type, void *event,
                   struct self *self)
{
    (void)chain;
    assert(self);

    if (likely(self->guard == 0))
        self_guard(CAPTURE_EVENT, type, event, self);
    else
        log_debug("!!! [%lu:0x%lx:%lu] %u_%u: %d", self_id(&self->md),
                  (uint64_t)self->pid, self->oid, chain, type, self->guard);

    assert(self->guard >= 0);
    return PS_STOP_CHAIN;
}

static struct self *
get_or_create_self_(bool publish)
{
    struct self *self = get_self_();
    if (likely(self)) {
        return self;
    }

    self = create_self_();

    thread_cache_set_(self);

    if (publish)
        self_handle_event_(CAPTURE_EVENT, EVENT_SELF_INIT, 0, self);
    return self;
}

PS_SUBSCRIBE(INTERCEPT_EVENT, ANY_TYPE, {
    return self_handle_event_(chain, type, event, get_or_create_self_(true));
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, ANY_TYPE, {
    if (type == EVENT_THREAD_CREATE)
        cleanup_threads_(0);
    return self_handle_before_(chain, type, event, get_or_create_self_(true));
})
PS_SUBSCRIBE(INTERCEPT_AFTER, ANY_TYPE, {
    return self_handle_after_(chain, type, event, get_or_create_self_(true));
})

// -----------------------------------------------------------------------------
// init, fini and registration
// -----------------------------------------------------------------------------

static void
self_fini_(struct self *self)
{
    if (self == NULL)
        return;

    // announce thread self is really over
    self_handle_event_(CAPTURE_EVENT, EVENT_SELF_FINI, 0, self);

    // free self resources
    destroy_self_(self);
}

static void
cleanup_threads_(pthread_t pid)
{
    caslock_acquire(&threads_.lock);
    struct quack_node_s *item = quack_popall(&threads_.retired);
    struct quack_node_s *next = NULL;

    for (; item; item = next) {
        next = item->next;

        struct self *self = container_of(item, struct self, retired_node);

        if ((pid != 0 && self->pid == pid) || (pid == 0 && thread_dead_(self)))
            self_fini_(self);
        else
            quack_push(&threads_.retired, item);
    }
    caslock_release(&threads_.lock);
}

PS_SUBSCRIBE(INTERCEPT_EVENT, EVENT_THREAD_EXIT, {
    struct self *self = get_or_create_self_(true);
    self_handle_event_(chain, type, event, self);
    retire_self_(self);
    return PS_STOP_CHAIN;
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_THREAD_JOIN, {
    struct self *self             = get_or_create_self_(true);
    struct pthread_join_event *ev = EVENT_PAYLOAD(ev);
    self_handle_after_(chain, type, event, self);
    return PS_STOP_CHAIN;
})

DICE_MODULE_INIT({ init_threads_(); })
DICE_MODULE_FINI({
    cleanup_threads_(0);
    self_fini_(get_self_());
})
