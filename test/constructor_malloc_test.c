#include <stdlib.h>

static void __attribute__((constructor))
bootstrap_malloc_(void)
{
    void *ptr = malloc(64);
    if (ptr == NULL)
        abort();
    free(ptr);
}

int
main(void)
{
    return 0;
}
