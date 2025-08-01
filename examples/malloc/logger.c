#include <dice/chains/capture.h>
#include <dice/events/malloc.h>
#include <dice/log.h>
#include <dice/module.h>
#include <dice/self.h>

DICE_MODULE_INIT()

PS_SUBSCRIBE(CAPTURE_BEFORE, EVENT_MALLOC, {
    struct malloc_event *ev = EVENT_PAYLOAD(ev);
    log_info("[logger:%lu] malloc(%lu) called at pc=%p", self_id(md), ev->size,
             ev->pc);
})

PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_MALLOC, {
    struct malloc_event *ev = EVENT_PAYLOAD(ev);
    log_info("[logger:%lu] malloc(%lu) returned %p", self_id(md), ev->size,
             ev->ret);
})

PS_SUBSCRIBE(CAPTURE_BEFORE, EVENT_FREE, {
    struct free_event *ev = EVENT_PAYLOAD(ev);
    log_info("[logger:%lu] free(%p) called", self_id(md), ev->ptr);
})
