#include <dice/compiler.h>

#if 0                           /* disabled */
DICE_HIDE void *
memset(void *s, int c, size_t n)
{
    volatile unsigned char *b = (volatile unsigned char *)s;
    for (; n != 0; n--) {
        b[n - 1] = c;
    }
    return s;
}
#endif
