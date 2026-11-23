#ifdef __NetBSD__
/* Match the CLI: NetBSD redirects POSIX rename to __posix_rename. */
    #define _XOPEN_SOURCE 700
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __APPLE__
    #include <dlfcn.h>
#endif

static int
fail_rename(const char *from, const char *to)
{
    static int failed;
    const char *target = getenv("DICE_TEST_RENAME_FAIL");
    if (!failed && target && !strcmp(to, target)) {
        failed = 1;
        errno  = EIO;
        return -1;
    }
#ifdef __APPLE__
    return rename(from, to);
#else
    #ifdef __NetBSD__
    const char *symbol = "__posix_rename";
    #else
    const char *symbol = "rename";
    #endif
    int (*real_rename)(const char *, const char *) = dlsym(RTLD_NEXT, symbol);
    return real_rename(from, to);
#endif
}

#ifdef __APPLE__
static const struct {
    const void *replacement;
    const void *original;
} rename_hook __attribute__((used, section("__DATA,__interpose"))) = {
    (const void *)fail_rename, (const void *)rename};
#else
int
rename(const char *from, const char *to)
{
    return fail_rename(from, to);
}
#endif
