#define DICE_MODULE_SLOT SLOT_SOURCE
#include <assert.h>
#include <string.h>

#include "dice.h"

void check_public_publish(void);

static enum ps_err
callback(chain_id chain, type_id type, void *event, struct metadata *md)
{
    (void)chain;
    (void)type;
    (void)md;
    *(int *)event += 1;
    return PS_OK;
}

static void *
worker(void *arg)
{
    return arg;
}

int
main(int argc, char **argv)
{
    check_public_publish();
    int n = 0;
    if (argc > 1) {
        /* Volatile inputs ensure these exercise dynamic validation. */
        volatile chain_id chain = CHAIN_OUTPUT;
        volatile type_id type   = EVENT_TEST;
        volatile int slot       = SLOT_OBSERVER;
        if (!strcmp(argv[1], "produces")) {
            PS_PUBLISH(chain, type, &n, NULL);
        } else if (!strcmp(argv[1], "event")) {
            type = EVENT_OTHER;
            (void)ps_publish(CHAIN_TEST, type, &n, NULL);
        } else if (!strcmp(argv[1], "consumes")) {
            chain = CHAIN_TEST;
            (void)ps_subscribe(chain, type, callback, slot);
        } else if (!strcmp(argv[1], "any")) {
            type = ANY_EVENT;
            (void)ps_subscribe(chain, type, callback, slot);
        } else if (!strcmp(argv[1], "slot")) {
            slot = 0;
            (void)ps_subscribe(chain, type, callback, slot);
        } else if (!strcmp(argv[1], "pointer")) {
            ps_callback_f publish = ps_publish;
            (void)publish(chain, type, &n, NULL);
        } else {
            return 2;
        }
        return 0;
    }
    assert(ps_publish(CHAIN_TEST, EVENT_TEST, &n, NULL) == PS_OK);
    assert(n == (getenv("WITH_PLUGIN") ? 102 : 2));
    assert(ps_subscribe(CHAIN_WILD, EVENT_TEST, callback, SLOT_WILDCARD) ==
           PS_OK);
    n = 0;
    PS_PUBLISH(CHAIN_WILD, EVENT_TEST, &n, NULL);
    assert(n == (getenv("WITH_PLUGIN") ? 101 : 1));
    PS_PUBLISH(CHAIN_WILD, EVENT_OTHER, &n, NULL);
    void *p = malloc(1234);
    assert(p);
    free(p);
    pthread_t thread;
    assert(pthread_create(&thread, NULL, worker, NULL) == 0);
    assert(pthread_join(thread, NULL) == 0);
    puts("route checks passed");
    return 0;
}
