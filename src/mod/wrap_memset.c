#include <dice/interpose.h>

void *dice___memset(void *, int, size_t);

void *
memset(void *ptr, int value, size_t num)
{
    return dice___memset(ptr, value, num);
}
FAKE_REAL_APPLE_DECL(memset_, memset, memset);
