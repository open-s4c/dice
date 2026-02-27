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
 * Self eventually garbage collect the self object of finished threads.
 */
#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#if defined(__NetBSD__)
    #include <errno.h>
    #include <lwp.h>
#elif defined(__APPLE__)
    #include <errno.h>
#endif

#ifndef DICE_MODULE_SLOT
    #define DICE_MODULE_SLOT 4
#endif
#include <dice/chains/intercept.h>
#include <dice/events/pthread.h>
#include <dice/mempool.h>
#include <dice/module.h>
#include <dice/pubsub.h>
#include <dice/self.h>
#include <vsync/atomic.h>
#include <vsync/spinlock/caslock.h>
#include <vsync/stack/quack.h>

#ifndef container_of
    #define container_of(A, T, M) ((T *)((((size_t)(A)) - offsetof(T, M))))
#endif

/* self object represents each thread in the CAPTURE chains. */
struct self {
    struct metadata md;
    struct quack_node_s retired_node;
    uint64_t osid;  // OS-specific thread identifier
    pthread_t ptid; // pthread-specific thread identifier
    thread_id id;   // Dice thread identifier \in 1..MAX_UINT64-1
    struct {
        struct tls_item *items;
        size_t size;
        size_t cap;
    } tls;
    int guard;
    bool retired;
};

/* tls_item is a memory object allocated in the thread-local storage (tls) */
struct tls_item {
    uintptr_t key;
    struct tls_dtor dtor;
    void *ptr;
};

static void cleanup_threads_(struct self *own, pthread_t ptid);
static struct self *get_self_(void);

// -----------------------------------------------------------------------------
// tls items
// -----------------------------------------------------------------------------

static void
tls_init_(struct self *self)
{
    self->tls.cap   = 1024;
    self->tls.size  = 0;
    self->tls.items = mempool_alloc(sizeof(struct tls_item) * self->tls.cap);
    if (self->tls.items == NULL)
        log_fatal("could not allocate self->tls");
}

static void
tls_fini_(struct self *self)
{
    for (size_t i = 0; i < self->tls.size; i++) {
        struct tls_item *item = &self->tls.items[i];
        if (item->dtor.free)
            item->dtor.free(item->dtor.arg, item->ptr);
        item->ptr = NULL;
    }
    self->tls.size = 0;

    // we cannot guarantee the main thread won't ever come back, so we keep the
    // pool active to the end of the program
    if (self->id != MAIN_THREAD) {
        self->tls.cap = 0;
        mempool_free(self->tls.items);
    }
}

static struct tls_item *
tls_find_(struct self *self, uintptr_t key)
{
    for (size_t i = 0; i < self->tls.size; i++) {
        struct tls_item *item = &self->tls.items[i];
        if (item->key == key)
            return item;
    }
    return NULL;
}

static void
tls_add_(struct self *self, struct tls_item item)
{
    assert(self->tls.cap > self->tls.size);
    self->tls.items[self->tls.size++] = item;
}

static void
tls_del_(struct self *self, uintptr_t key)
{
    for (size_t i = 0; i < self->tls.size; i++) {
        struct tls_item *item = &self->tls.items[i];
        if (item->key == key) {
            if (item->dtor.free)
                item->dtor.free(item->dtor.arg, item->ptr);

            if (self->tls.size > 1 && i < self->tls.size - 1)
                self->tls.items[i] = self->tls.items[self->tls.size - 1];

            self->tls.size--;
            return;
        }
    }
}

// -----------------------------------------------------------------------------
// public interface
// -----------------------------------------------------------------------------

DICE_HIDE thread_id
self_id_(struct metadata *md)
{
    struct self *self = (struct self *)md;
    return likely(self) ? self->id : NO_THREAD;
}

DICE_HIDE bool
self_retired_(struct metadata *md)
{
    struct self *self = (struct self *)md;
    return likely(self) ? self->retired : false;
}

DICE_HIDE void *
self_tls_get_(struct metadata *md, uintptr_t item_key)
{
    struct self *self = (struct self *)md;
    assert(self != NULL);

    struct tls_item *item = tls_find_(self, item_key);
    return item ? item->ptr : NULL;
}

DICE_HIDE void
self_tls_set_(struct metadata *md, uintptr_t item_key, void *ptr,
              struct tls_dtor dtor)
{
    struct self *self = (struct self *)md;
    assert(self != NULL);

    if (ptr == NULL) {
        tls_del_(self, item_key);
        return;
    }

    struct tls_item *old = tls_find_(self, item_key);

    struct tls_item item = {
        .key  = item_key,
        .dtor = dtor,
        .ptr  = ptr,
    };
    if (old)
        *old = item;
    else
        tls_add_(self, item);
}

static void
mempool_free_dtor_(void *arg, void *ptr)
{
    (void)arg;
    mempool_free(ptr);
}

DICE_HIDE void *
self_tls_(struct metadata *md, const void *global, size_t size)
{
    uintptr_t item_key = (uintptr_t)global;

    void *data = self_tls_get(md, item_key);
    if (likely(data))
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

DICE_WEAK thread_id
self_id(struct metadata *md)
{
    return self_id_(md);
}

DICE_WEAK bool
self_retired(struct metadata *md)
{
    return self_retired_(md);
}

DICE_WEAK void *
self_tls_get(struct metadata *md, uintptr_t item_key)
{
    return self_tls_get_(md, item_key);
}

DICE_WEAK void
self_tls_set(struct metadata *md, uintptr_t item_key, void *ptr,
             struct tls_dtor dtor)
{
    self_tls_set_(md, item_key, ptr, dtor);
}

DICE_WEAK void *
self_tls(struct metadata *md, const void *global, size_t size)
{
    return self_tls_(md, global, size);
}

DICE_WEAK struct metadata *
self_md(void)
{
    return (struct metadata *)get_self_();
}

// -----------------------------------------------------------------------------
// thread cache using pthread_get/setspecific
// -----------------------------------------------------------------------------

static struct {
    caslock_t lock;
    vatomic64_t count;
    vatomic64_t created;
    vatomic64_t dead;
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
thread_cache_set_(struct self *self)
{
    if (pthread_setspecific(threads_.cache_key, self) != 0)
        log_fatal("could not set key");
}

static inline struct self *
thread_cache_get_(void)
{
    return (struct self *)pthread_getspecific(threads_.cache_key);
}

#if defined(__linux__)

static uint64_t
thread_osid_(void)
{
    return (uint64_t)gettid();
}
static bool
thread_dead_(struct self *self)
{
    return kill((pid_t)self->osid, 0) != 0;
}

#elif defined(__NetBSD__)

static uint64_t
thread_osid_(void)
{
    return _lwp_self();
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
    return pthread_kill(self->ptid, 0) == ESRCH;
}

#elif defined(__APPLE__)

static uint64_t
thread_osid_(void)
{
    return (uint64_t)pthread_mach_thread_np(pthread_self());
}

static bool
thread_dead_(struct self *self)
{
    (void)self;
    return pthread_kill(self->ptid, 0) == ESRCH;
}
#endif // !linux

static struct self *
create_self_()
{
    struct self *self;
    self          = mempool_alloc(sizeof(struct self));
    self->guard   = 0;
    self->id      = vatomic64_inc_get(&threads_.count);
    self->ptid    = pthread_self();
    self->osid    = thread_osid_();
    self->retired = false;
    tls_init_(self);
    return self;
}

static void
destroy_self_(struct self *self)
{
    tls_fini_(self);

    // The main thread might still intercept after the module has
    // terminated. So, we keep the self object allocated to avoid issues.
    if (self->id != MAIN_THREAD)
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
quack_search_(pthread_t ptid)
{
    // We now search the retired stack. To avoid not seeing our self object
    // while other threads are searching theirs, we have to ensure mutual
    // exlusion here.
    struct self *self = NULL;
    caslock_acquire(&threads_.lock);
    struct quack_node_s *item = quack_popall(&threads_.retired);
    struct quack_node_s *next = NULL;

    for (; item; item = next) {
        next = item->next;

        struct self *it = container_of(item, struct self, retired_node);

        uint64_t osid = thread_osid_();
        if (self == NULL && it->ptid == ptid && it->osid == osid)
            self = it;
        quack_push(&threads_.retired, item);
    }
    caslock_release(&threads_.lock);
    assert(self == NULL || self->retired);
    return self;
}

static inline struct self *
get_self_(void)
{
    // When a thread is created, it won't find its self object anywhere and
    // this function will return NULL. A new self object will be created and
    // stored in the thread specific area (here called cache).

    // When a thread is retired -- ie, between  THREAD_EXIT and SELF_FINI --
    // its self object is put into the retired stack. The object is still
    // kept in the thread_cache.

    // So when searching for the self object, we first look in the cache
    // (ie, the thread specific area provided by pthread). The pthread
    // implementation is expected to zero that thread specific area when
    // starting a new thread, and (in some systems) it also does it when
    // terminating the thread. When the latter happens, we need to look for
    // the thread in the retired stack.

    // Since pthread_self ids can be reused, we use the pair (ptid,osid) ids
    // as unique identifier.

    struct self *self = thread_cache_get_();
    if (likely(self))
        return self;

    return quack_search_(pthread_self());
}

static void
retire_self_(struct self *self)
{
    assert(self);
    assert(!self->retired);
    self->retired = true;
    quack_push(&threads_.retired, &self->retired_node);
}

// -----------------------------------------------------------------------------
//  pubsub handler
// -----------------------------------------------------------------------------

#define self_guard(chain, type, event, self)                                   \
    do {                                                                       \
        self->guard++;                                                         \
        log_debug(">> [%" PRIu64 ":0x%" PRIx64 ":%" PRIu64 "] %s/%s: %d",      \
                  self_id(&self->md), (uint64_t)self->ptid, self->osid,        \
                  ps_chain_str(chain), ps_type_str(type), self->guard);        \
        PS_PUBLISH(chain, type, event, &self->md);                             \
        log_debug("<< [%" PRIu64 ":0x%" PRIx64 ":%" PRIu64 "] %s/%s: %d",      \
                  self_id(&self->md), (uint64_t)self->ptid, self->osid,        \
                  ps_chain_str(chain), ps_type_str(type), self->guard);        \
        self->guard--;                                                         \
    } while (0)

static enum ps_err
self_handle_before_(const chain_id chain, const type_id type, void *event,
                    struct self *self)
{
    (void)chain;
    assert(self);

    if (likely(self->guard++ == 0))
        self_guard(CAPTURE_BEFORE, type, event, self);
    else
        log_debug(">>> [%" PRIu64 ":0x%" PRIx64 ":%" PRIu64 "] %s/%s: %d",
                  self_id(&self->md), (uint64_t)self->ptid, self->osid,
                  ps_chain_str(chain), ps_type_str(type), self->guard);

    assert(self->guard >= 0);
    return self->md.drop ? PS_DROP_EVENT : PS_STOP_CHAIN;
}

static enum ps_err
self_handle_after_(const chain_id chain, const type_id type, void *event,
                   struct self *self)
{
    (void)chain;
    assert(self);

    if (likely(self->guard-- == 1))
        self_guard(CAPTURE_AFTER, type, event, self);
    else
        log_debug("<<< [%" PRIu64 ":0x%" PRIx64 ":%" PRIu64 "] %s/%s: %d",
                  self_id(&self->md), (uint64_t)self->ptid, self->osid,
                  ps_chain_str(chain), ps_type_str(type), self->guard);

    assert(self->guard >= 0);
    return self->md.drop ? PS_DROP_EVENT : PS_STOP_CHAIN;
}

static enum ps_err
self_handle_event_(const chain_id chain, const type_id type, void *event,
                   struct self *self)
{
    (void)chain;
    assert(self);

    if (likely(self->guard == 0))
        self_guard(CAPTURE_EVENT, type, event, self);
    else
        log_debug("!!! [%" PRIu64 ":0x%" PRIx64 ":%" PRIu64 "] %s/%s: %d",
                  self_id(&self->md), (uint64_t)self->ptid, self->osid,
                  ps_chain_str(chain), ps_type_str(type), self->guard);

    assert(self->guard >= 0);
    return self->md.drop ? PS_DROP_EVENT : PS_STOP_CHAIN;
}

static inline struct self *
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

PS_SUBSCRIBE(INTERCEPT_EVENT, ANY_EVENT, {
    struct self *self = get_or_create_self_(true);
    self->md          = md ? *md : (struct metadata){0};

    return self_handle_event_(chain, type, event, self);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, ANY_EVENT, {
    struct self *self = get_or_create_self_(true);
    self->md          = md ? *md : (struct metadata){0};

    if (unlikely(type == EVENT_THREAD_CREATE))
        cleanup_threads_(self, 0);
    return self_handle_before_(chain, type, event, self);
})
PS_SUBSCRIBE(INTERCEPT_AFTER, ANY_EVENT, {
    struct self *self = get_or_create_self_(true);
    self->md          = md ? *md : (struct metadata){0};

    if (unlikely(type == EVENT_THREAD_CREATE)) {
        struct pthread_create_event *ev = EVENT_PAYLOAD(ev);
        if (ev->ret == 0)
            vatomic_inc(&threads_.created);
    };
    return self_handle_after_(chain, type, event, self);
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
cleanup_threads_(struct self *own, pthread_t ptid)
{
    // Once the current thread calls self_fini_() below, it will emit a message
    // on the behalf of another thread. The subscribers might be again
    // intercepted by Dice. When that happens, this thread will query for its
    // self object without knowing it is finilizing another thread. To stop the
    // recursion we use the same guard mechanism of normal publications.
    if (own)
        own->guard++;

    caslock_acquire(&threads_.lock);
    struct quack_node_s *item = quack_popall(&threads_.retired);
    struct quack_node_s *next = NULL;

    for (; item; item = next) {
        next = item->next;

        struct self *self = container_of(item, struct self, retired_node);

        if ((ptid != 0 && self->ptid == ptid) ||
            (ptid == 0 && thread_dead_(self))) {
            self_fini_(self);
            vatomic_inc(&threads_.dead);
        } else {
            quack_push(&threads_.retired, item);
        }
    }
    caslock_release(&threads_.lock);
    if (own)
        own->guard--;
}

PS_SUBSCRIBE(INTERCEPT_EVENT, EVENT_THREAD_EXIT, {
    struct self *self = get_or_create_self_(true);
    self->md          = md ? *md : (struct metadata){0};

    self_handle_event_(chain, type, event, self);
    retire_self_(self);
    return self->md.drop ? PS_DROP_EVENT : PS_STOP_CHAIN;
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_THREAD_JOIN, {
    struct self *self = get_or_create_self_(true);
    self->md          = md ? *md : (struct metadata){0};

    struct pthread_join_event *ev = EVENT_PAYLOAD(ev);
    self_handle_after_(chain, type, event, self);
    return self->md.drop ? PS_DROP_EVENT : PS_STOP_CHAIN;
})

DICE_MODULE_INIT({ init_threads_(); })
DICE_MODULE_FINI({
    struct self *self = get_self_();
    thread_id tid     = self ? self->id : MAIN_THREAD;

    if (vatomic_read(&threads_.count) == 0) {
        log_warn(
            "threads_.count==0: "
            "main thread did not increment count?");
    }

    uint64_t born = 0;
    uint64_t dead = 0;

    do {
        cleanup_threads_(self, 0);
        dead = vatomic_read(&threads_.dead);
        born = vatomic_read(&threads_.created);
        if (tid == MAIN_THREAD)
            log_debug("waiting for %" PRIu64 " threads to die", (born - dead));
    } while (tid == MAIN_THREAD && dead < born);

    self_fini_(self);
})

PS_ADVERTISE_TYPE(EVENT_SELF_INIT)
PS_ADVERTISE_TYPE(EVENT_SELF_FINI)
PS_ADVERTISE_CHAIN(CAPTURE_EVENT)
PS_ADVERTISE_CHAIN(CAPTURE_BEFORE)
PS_ADVERTISE_CHAIN(CAPTURE_AFTER)
