#include <assert.h>
#include <dlfcn.h>
#include <stdlib.h>

int sealed_probe(void);

static void
check(const char *const names[], size_t count, int sealed)
{
    for (size_t i = 0; i < count; i++)
        assert((dlsym(RTLD_DEFAULT, names[i]) == NULL) == !!sealed);
}

int
main(int argc, char **argv)
{
    assert(argc == 2);
    unsigned mask        = (unsigned)atoi(argv[1]);
    const char *pubsub[] = {"ps_publish",        "ps_subscribe",
                            "ps_register_chain", "ps_register_type",
                            "ps_chain_str",      "ps_type_str",
                            "ps_chain_lookup",   "ps_type_lookup",
                            "dice_require_abi",  "dice_subscribe_abi"};
    const char *pool[]   = {"mempool_alloc", "mempool_realloc", "mempool_free",
                            "mempool_aligned_alloc"};
    const char *self[]   = {"self_id",      "self_guard_get", "self_retired",
                            "self_tls_get", "self_tls_set",   "self_tls",
                            "self_md"};
    check(pubsub, sizeof(pubsub) / sizeof(*pubsub), mask & 1);
    check(pool, sizeof(pool) / sizeof(*pool), mask & 2);
    check(self, sizeof(self) / sizeof(*self), mask & 4);
    return sealed_probe();
}
