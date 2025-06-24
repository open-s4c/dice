/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 * -----------------------------------------------------------------------------
 * Thread "self-awareness"
 *
 * This module implements TLS management with the memory from mempool.h and
 * guards further modules from receiving events before threads are
 * initialized or after they are finished.
 *
 * The module subscribes to all chains of the pubsub and should be
 * initialized as first module so that it is placed at the start of each
 * chain. The module blocks any events of a thread until the thread
 * publishes a THREAD_INIT event. It also blocks any event of a thread after
 * it sends a THREAD_FINI event.
 *
 * This should be the first module subscribing to all events from pubsub.
 */
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#define DICE_MODULE_PRIO 4
#include <dice/chains/intercept.h>
#include <dice/log.h>
#include <dice/mempool.h>
#include <dice/module.h>
#include <dice/pubsub.h>
#include <dice/rbtree.h>
#include <dice/self.h>
#include <dice/thread_id.h>
#include <vsync/atomic.h>

typedef struct thread_data {
    metadata_t md;
    int guard;
    thread_id tid;
    int count;
    struct rbtree tree;
} thrdata_t;

struct tls_item {
    uintptr_t key;
    struct rbnode node;
    char data[];
};

DICE_HIDE vatomic64_t _thread_count = VATOMIC_INIT(0);

DICE_HIDE void _self_destruct(void *);
DICE_HIDE void _tls_init(thrdata_t *td);

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
_tls_init(thrdata_t *td)
{
    rbtree_init(&td->tree, _tls_cmp);
}

DICE_HIDE void
_tls_fini(thrdata_t *td)
{
    // TODO: iterate over all items and mempool_free them
    (void)td;
}

// -----------------------------------------------------------------------------
// thrmap and thrdata using pthread_get/setspecific
// -----------------------------------------------------------------------------
static pthread_key_t _key;

static void
_thrmap_init(void)
{
    if (pthread_key_create(&_key, _self_destruct) != 0)
        abort();
}
static void
_thrmap_fini(void)
{
}

DICE_HIDE thrdata_t *
_thrdata_get(void)
{
    return (thrdata_t *)pthread_getspecific(_key);
}
DICE_HIDE thrdata_t *
_thrdata_new()
{
    thrdata_t *td = mempool_alloc(sizeof(thrdata_t));
    td->guard     = 0;
    td->count     = 0;
    td->tid       = vatomic64_inc_get(&_thread_count);
    _tls_init(td);

    if (pthread_setspecific(_key, td) != 0)
        log_fatalf("could not set key");
    return td;
}

static void
_thrdata_del(thrdata_t *td)
{
    (void)td;
    if (pthread_setspecific(_key, 0) != 0)
        abort();
}

// -----------------------------------------------------------------------------
// public interface
// -----------------------------------------------------------------------------
DICE_HIDE_IF thread_id
self_id(metadata_t *md)
{
    thrdata_t *td = (thrdata_t *)md;
    return td ? td->tid : NO_THREAD;
}

DICE_HIDE_IF void *
self_tls(metadata_t *md, const void *global, size_t size)
{
    uintptr_t item_key = (uintptr_t)global;
    thrdata_t *td      = (thrdata_t *)md;

    // should never be called before the self initialization
    assert(td != NULL);

    struct tls_item key = {.key = item_key};
    struct rbnode *item = rbtree_find(&td->tree, &key.node);

    if (item) {
        struct tls_item *i = container_of(item, struct tls_item, node);
        return i->data;
    }

    struct tls_item *i = mempool_alloc(size + sizeof(struct tls_item));
    if (i == NULL)
        return NULL;
    memset(&i->data, 0, size);
    i->key = item_key;
    rbtree_insert(&td->tree, &i->node);
    return i->data;
}

// -----------------------------------------------------------------------------
// self destructor
// -----------------------------------------------------------------------------

DICE_HIDE void
_self_destruct(void *arg)
{
    // In some systems (eg, NetBSD) threads still call interceptors while
    // terminating (inside pthread_exit). Setting _key to NULL instructs the
    // pubsub to ignore further events. No further TLS data will be
    // (re)created because no EVENT_THREAD_INIT will be published for this
    // thread anymore.
    if (arg == NULL)
        return;

    // TODO: change order of atexit destructors

    _thrdata_del(arg);
    _tls_fini(arg);
    mempool_free(arg);
}

//  -----------------------------------------------------------------------------
//  pubsub handler
//  -----------------------------------------------------------------------------
#define self_guard(chain, type, event, td)                                     \
    do {                                                                       \
        td->guard++;                                                           \
        td->count++;                                                           \
        td->md = (metadata_t){0};                                              \
        PS_PUBLISH(chain, type, event, &td->md);                               \
        td->guard--;                                                           \
    } while (0)

DICE_HIDE enum ps_cb_err
_self_handle_before(const chain_id chain, const type_id type, void *event,
                    metadata_t *md)
{
    (void)chain;
    (void)md;
    thrdata_t *td = _thrdata_get();
    if (unlikely(td == NULL))
        return PS_CB_STOP;

    if (likely(td->guard++ == 0))
        self_guard(CAPTURE_BEFORE, type, event, td);

    return PS_CB_STOP;
}

DICE_HIDE enum ps_cb_err
_self_handle_after(const chain_id chain, const type_id type, void *event,
                   metadata_t *md)
{
    (void)chain;
    (void)md;
    thrdata_t *td = _thrdata_get();
    if (unlikely(td == NULL))
        return PS_CB_STOP;

    if (likely(td->guard-- == 1))
        self_guard(CAPTURE_AFTER, type, event, td);

    return PS_CB_STOP;
}

DICE_HIDE enum ps_cb_err
_self_handle_event(const chain_id chain, const type_id type, void *event,
                   metadata_t *md)
{
    (void)chain;
    (void)md;
    thrdata_t *td = _thrdata_get();

    if (unlikely(td == NULL))
        return PS_CB_STOP;

    if (unlikely(td->tid == MAIN_THREAD && td->count == 0)) {
        self_guard(CAPTURE_EVENT, EVENT_THREAD_INIT, 0, td);
    }

    if (likely(td->guard == 0))
        self_guard(CAPTURE_EVENT, type, event, td);

    return PS_CB_STOP;
}

PS_SUBSCRIBE(INTERCEPT_EVENT, EVENT_THREAD_INIT, {
    // Only initialize TLS if the event is a THREAD_INIT event.
    thrdata_t *td = _thrdata_new();
    self_guard(CAPTURE_EVENT, EVENT_THREAD_INIT, 0, td);
    return PS_CB_STOP;
})
PS_SUBSCRIBE(INTERCEPT_EVENT, EVENT_THREAD_FINI, {
    thrdata_t *td = _thrdata_get();

    /* EVENT_THREAD_FINI could be published multiple times depending how the
     * pthread library implements the exit of the thread. For example, in
     * NetBSD 10.1, pthread_exit is called after the thread returns from its run
     * routine. */
    if (td) {
        self_guard(CAPTURE_EVENT, EVENT_THREAD_FINI, 0, td);
        _self_destruct(td);
    }
    return PS_CB_STOP;
})

PS_SUBSCRIBE(INTERCEPT_EVENT, ANY_TYPE,
             { return _self_handle_event(chain, type, event, md); })
PS_SUBSCRIBE(INTERCEPT_BEFORE, ANY_TYPE,
             { return _self_handle_before(chain, type, event, md); })
PS_SUBSCRIBE(INTERCEPT_AFTER, ANY_TYPE,
             { return _self_handle_after(chain, type, event, md); })


// -----------------------------------------------------------------------------
// init, fini and registration
// -----------------------------------------------------------------------------

DICE_MODULE_INIT({
    // First, prepare the map for TLS
    _thrmap_init();

    // Construct TLS for main thread, but do not publish any event yet. On the
    // first event of the main thread (handled by self's callback), we piggyback
    // a EVENT_THREAD_INIT for the main thread.
    (void)_thrdata_new();
})

DICE_MODULE_FINI({
    thrdata_t *td = _thrdata_get();
    PS_PUBLISH(CAPTURE_EVENT, EVENT_THREAD_FINI, 0, &td->md);
    _thrmap_fini();
})
