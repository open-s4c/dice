#include <stdarg.h>
#include <dice/utils.h>

int snprintf_s(char *s, size_t n, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vsnprintf(s, n, fmt, ap);
    va_end(ap);
    if (ret < 0 || ret >= (int)n) {
        if (n > 0) s[0] = '\0'; // Mandatory Annex K behavior on error
        return -1;
    }
    return ret;
}
