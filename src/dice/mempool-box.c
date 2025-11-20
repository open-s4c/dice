#include <dice/compiler.h>
#include <dice/mempool.h>

void *mempool_alloc_(size_t size);
void *mempool_realloc_(void *ptr, size_t size);
void mempool_free_(void *ptr);

DICE_HIDE void *
mempool_alloc(size_t size)
{
    return mempool_alloc_(size);
}

DICE_HIDE void *
mempool_realloc(void *ptr, size_t size)
{
    return mempool_realloc_(ptr, size);
}

DICE_HIDE void
mempool_free(void *ptr)
{
    return mempool_free_(ptr);
}
