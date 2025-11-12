/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
/*******************************************************************************
 * @file stacktrace.c
 * @brief Publishes func_entry and func_exit events from TSAN instrumentation.
 *
 ******************************************************************************/
#include <stdint.h>

#include <dice/chains/intercept.h>
#include <dice/events/stacktrace.h>
#include <dice/events/thread.h>
#include <dice/interpose.h>
#include <dice/module.h>
#include <dice/pubsub.h>

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_STACKTRACE_ENTER)
PS_ADVERTISE_TYPE(EVENT_STACKTRACE_EXIT)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()

static inline void check_main_start_(void *caller);

void
__tsan_func_entry(void *caller)
{
    check_main_start_(INTERPOSE_PC);
    stacktrace_event_t ev = {.caller = caller, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_STACKTRACE_ENTER, &ev, &md);
}
void
__tsan_func_exit(void)
{
    stacktrace_event_t ev = {.pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_STACKTRACE_EXIT, &ev, &md);
}

/* -----------------------------------------------------------------------------
 * main thread start and exit of main() function
 * -------------------------------------------------------------------------- */
#if !defined(DICE_MAIN_START_DISABLE)
static void
main_exit_()
{
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_THREAD_EXIT, 0, 0);
    log_debug("main thread exits");
}
static void
main_start_()
{
    log_debug("main thread starts");
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_THREAD_START, 0, 0);
    atexit(main_exit_);
}
#endif

/* -----------------------------------------------------------------------------
 * find main function address and distance calculator
 * -------------------------------------------------------------------------- */
#if defined(DICE_MAIN_START_DISABLE)

static inline void
check_main_start_(void *retpc)
{
    (void)retpc;
}

#elif defined(__NetBSD__)
    #include <dlfcn.h>
    #include <limits.h>
    #include <link.h>
    #include <nlist.h>
    #include <stdint.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>

    #include <sys/stat.h>

static int
phdr_cb_(struct dl_phdr_info *info, size_t size, void *data)
{
    (void)size;
    const char *exe = (const char *)data;
    if (!info->dlpi_name || !info->dlpi_name[0])
        return 1 + (uintptr_t)(info->dlpi_addr != 0); // stop; base in dlpi_addr
    if (exe && strcmp(info->dlpi_name, exe) == 0)
        return 1 + (uintptr_t)info->dlpi_addr;
    return 0;
}

static void *
find_main_address_(void)
{
    char exe[PATH_MAX];
    ssize_t n = readlink("/proc/curproc/exe", exe, sizeof(exe) - 1);
    if (n <= 0)
        return NULL;
    exe[n] = '\0';

    // 1) link-time address from the file’s symbol table
    struct nlist nl[2] = {0};
    nl[0].n_name       = (char *)"main"; // ELF: no leading underscore
    if (nlist(exe, nl) != 0 || nl[0].n_type == 0 || nl[0].n_value == 0)
        return NULL; // stripped binary or main not present in .symtab

    uintptr_t linktime = (uintptr_t)nl[0].n_value;

    // 2) runtime slide (ASLR) of the main executable
    uintptr_t slide = 0;
    int rc          = dl_iterate_phdr(phdr_cb_, exe);
    if (rc > 1)
        slide = (uintptr_t)(rc - 1); // see phdr_cb trick above

    return (void *)(linktime + slide);
}


    #define MAX_MAIN_THREAD_DISTANCE 128
static inline void
check_main_start_(void *retpc)
{
    static void *main_ptr_ = NULL;
    static bool started_   = false;
    if (main_ptr_ == NULL)
        main_ptr_ = find_main_address_();


    if (!started_) {
        uintptr_t diff = (uintptr_t)retpc - (uintptr_t)main_ptr_;
        log_debug("diff: 0x%lx=%lu", diff, diff);
        if (diff < MAX_MAIN_THREAD_DISTANCE) {
            started_ = true;
            main_start_();
        }
    }
}

#elif defined(__linux__)

static inline void
check_main_start_(void *retpc)
{
    (void)retpc;
}


// glibc signature (LSB): __libc_start_main calls init, then main, then exit.
// https://refspecs.linuxbase.org/.../__libc_start_main-.html
typedef int (*main_f)(int, char **, char **);

    #if defined(__GLIBC__)
        #include <dlfcn.h>
        #include <stdint.h>
        #include <stdlib.h>

INTERPOSE(int, __libc_start_main, main_f mainf, int argc, char **ubp_av,
          void (*init)(void), void (*fini)(void), void (*rtld_fini)(void),
          void *stack_end)
{
    main_start_();
    return REAL(__libc_start_main, mainf, argc, ubp_av, init, fini, rtld_fini,
                stack_end);
}

    #else  // but no GLIBC, eg, Alpine

INTERPOSE(int, __libc_start_main, main_f mainf, int argc, char **argv,
          void (*init)(void), void (*fini)(void), void (*rtld_fini)(void))
{
    main_start_();
    return REAL(__libc_start_main, mainf, argc, argv, init, fini, rtld_fini);
}
    #endif // defined(__linux__)
#endif
