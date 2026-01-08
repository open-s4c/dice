#define LOG_PREFIX "TID_CHECKER: "
#ifndef LOG_LEVEL
    #define LOG_LEVEL INFO
#endif

#include <dice/chains/capture.h>
#include <dice/ensure.h>
#include <dice/events/self.h>
#include <dice/log.h>
#include <dice/module.h>
#include <dice/self.h>

#if defined(__NetBSD__)
    #include <lwp.h>
static uint64_t
osid_(void)
{
    return (uint64_t)_lwp_self();
}
#elif defined(__linux__)
static uint64_t
osid_(void)
{
    return (uint64_t)gettid();
}
#elif defined(__APPLE__)
    #include <pthread.h>
static uint64_t
osid_(void)
{
    return (uint64_t)pthread_mach_thread_np(pthread_self());
}
#endif

static uint64_t
self_osid_(struct metadata *md)
{
    uint8_t *ptr = (uint8_t *)md;   // start with the self object
    ptr += sizeof(struct metadata); // add metadata field
    ptr += sizeof(void *);          // add quack_node, which is a pointer
    uint64_t *osid_field = (uint64_t *)ptr;
    return *osid_field;
}

static void
check_ids_(chain_id chain, type_id type, struct metadata *md)
{
    uint64_t mdid = self_osid_(md);
    uint64_t osid = osid_();
    if (mdid != 0 && mdid != osid) {
        log_fatal("[%" PRIu64 " %s/%s] unexpected OS-based id %" PRIu64
                  " != %" PRIu64,
                  self_id(md), ps_chain_str(chain), ps_type_str(type), mdid,
                  osid);
    }
}

PS_SUBSCRIBE(CAPTURE_EVENT, ANY_EVENT, {
    // we can check for every event except SELF_FINI, where the current osid
    // will not match because another thread reclaims the self object when the
    // thread is already dead.
    if (type != EVENT_SELF_FINI)
        check_ids_(chain, type, md);
})
PS_SUBSCRIBE(CAPTURE_BEFORE, ANY_EVENT, { check_ids_(chain, type, md); })
PS_SUBSCRIBE(CAPTURE_AFTER, ANY_EVENT, { check_ids_(chain, type, md); })
