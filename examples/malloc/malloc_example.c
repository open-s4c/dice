#include <stdlib.h>

int
main(void)
{
    void *ptr = malloc(123);

    if (ptr)
        free(ptr);

    return 0;
}
