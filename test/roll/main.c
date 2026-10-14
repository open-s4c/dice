#include <assert.h>

#include "dice.h"
static void *
worker(void *arg)
{
    return arg;
}
int
main(void)
{
    int n = 0;
    assert(ps_publish(CHAIN_TEST, EVENT_TEST, &n, NULL) == PS_OK);
    assert(n == (getenv("WITH_PLUGIN") ? 1101 : 101));
    n = -1;
    assert(ps_publish(CHAIN_TEST, EVENT_TEST, &n, NULL) == PS_STOP_CHAIN);
    assert(n == -1);
    n = 0;
    assert(ps_publish(CHAIN_TEST, EVENT_UNUSED, &n, NULL) == PS_OK);
    assert(n == 110);
    n = 0;
    assert(ps_publish(CHAIN_EMPTY, EVENT_UNUSED, &n, NULL) == PS_OK);
    assert(n == (getenv("WITH_PLUGIN") ? 2000 : 0));
    assert(ps_publish(CHAIN_TEST, ANY_EVENT, NULL, NULL) == PS_INVALID);
    assert(ps_publish(MAX_CHAINS, EVENT_TEST, NULL, NULL) == PS_INVALID);
    assert(ps_publish(CHAIN_TEST, MAX_TYPES, NULL, NULL) == PS_INVALID);
    assert(ps_type_lookup("EVENT_TEST") == EVENT_TEST);
    assert(ps_chain_lookup("CHAIN_TEST") == CHAIN_TEST);
    void *p = malloc(1234);
    assert(p);
    free(p);
    pthread_t thread;
    assert(pthread_create(&thread, NULL, worker, NULL) == 0);
    assert(pthread_join(thread, NULL) == 0);
    puts("passed");
}
