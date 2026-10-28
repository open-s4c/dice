#include <assert.h>
#include <stdint.h>
#include <string.h>

#define DICE_MODULE_SLOT SLOT_APP
#include "dice.h"

static unsigned observed, callbacks_;
static unsigned key, tls_key;

PS_SUBSCRIBE(CAPTURE_EVENT, EVENT_TEST, {
    assert(self_id(md) != NO_THREAD);
    assert(self_md() == md);
    assert(self_guard_get(md) == SELF_GUARD_SERVING);
    assert(!self_retired(md));
    assert(self_tls_get(md, (uintptr_t)&key) == NULL);
    self_tls_set(md, (uintptr_t)&key, &key, (struct tls_dtor){0});
    assert(self_tls_get(md, (uintptr_t)&key) == &key);
    self_tls_set(md, (uintptr_t)&key, NULL, (struct tls_dtor){0});
    unsigned *value = self_tls(md, &tls_key, sizeof(*value));
    assert(value && *value == 0);
    assert(self_tls(md, &tls_key, sizeof(*value)) == value);
    observed = *(unsigned *)event;
})

static enum ps_err
callback(chain_id chain, type_id type, void *event, struct metadata *md)
{
    (void)chain;
    (void)type;
    (void)event;
    assert(self_id(md) != NO_THREAD);
    callbacks_++;
    return PS_OK;
}

int
sealed_probe(void)
{
    void *ptr = mempool_alloc(32);
    assert(ptr);
    memset(ptr, 0x5a, 32);
    ptr = mempool_realloc(ptr, 64);
    assert(ptr && ((unsigned char *)ptr)[0] == 0x5a);
    mempool_free(ptr);
    ptr = mempool_aligned_alloc(64, 32);
    assert(ptr && (uintptr_t)ptr % 64 == 0);
    mempool_free(ptr);
    int err = ps_subscribe(CAPTURE_EVENT, EVENT_TEST, callback,
                           DICE_LAST_KNOWN_SLOT + 1);
    assert(err == (DICE_EXTENSION_CALLBACKS ? PS_OK : PS_INVALID));
    unsigned value = 42;
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_TEST, &value, NULL);
    assert(observed == value);
    assert(callbacks_ == DICE_EXTENSION_CALLBACKS);
    return 0;
}
