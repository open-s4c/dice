#include "dice/types.h"
#include "tweaks.h"
#include <dice/pubsub.h>

// -----------------------------------------------------------------------------
// pubsub interface
// -----------------------------------------------------------------------------

bool ps_initd_(void);
enum ps_err ps_dispatch_(const chain_id, const type_id, void *,
                         struct metadata *);
int ps_dispatch_max(void);
bool ps_dispatch_chain_on_(chain_id);

DICE_HIDE enum ps_err
ps_publish(const chain_id chain, const type_id type, void *event,
           struct metadata *md)
{
    if (PS_NOT_INITD_())
        return PS_DROP_EVENT;

    log_debug("Dispatch %s/%s", ps_chain_str(chain), ps_type_str(type));
    enum ps_err err = ps_dispatch_(chain, type, event, md);

    if (likely(err == PS_STOP_CHAIN))
        return PS_OK;

    if (likely(err == PS_DROP_EVENT))
        return PS_DROP_EVENT;

    return PS_OK;
}

DICE_HIDE int
ps_subscribe(chain_id chain, type_id type, ps_callback_f cb, int slot)
{
    (void)type;
    (void)cb;
    if (ps_dispatch_chain_on_(chain) && slot <= ps_dispatch_max()) {
        // Ignore the subscription of dispatch handlers. publications reach them
        // via dispatch functions.
        return PS_OK;
    }

    if (chain == CHAIN_CONTROL) {
        // Ignore any subscription of callback handlers for control chain. This
        // is only valid for builtin modules.
        return PS_OK;
    }

    // Abort if trying to subscribe a non-dispatch handler. Abort early with
    // precise error message.
    log_fatal("cannot subscribe callback: library compiled with pubsub-box");
    return PS_ERROR;
}

// -----------------------------------------------------------------------------
// registry interface
// -----------------------------------------------------------------------------

void ps_registry_add_(bool chain, uint16_t id, const char *name);
const char *ps_registry_get_(bool chain, uint16_t id);
uint16_t ps_registry_lookup_(bool chain, const char *name);

DICE_HIDE void
ps_register_chain(chain_id id, const char *name)
{
    ps_registry_add_(true, id, name);
}
DICE_HIDE void
ps_register_type(type_id id, const char *name)
{
    return ps_registry_add_(false, id, name);
}
DICE_HIDE const char *
ps_chain_str(chain_id id)
{
    return ps_registry_get_(true, id);
}
DICE_HIDE const char *
ps_type_str(type_id id)
{
    return ps_registry_get_(false, id);
}
DICE_HIDE chain_id
ps_chain_lookup(const char *name)
{
    return ps_registry_lookup_(true, name);
}
DICE_HIDE type_id
ps_type_lookup(const char *name)
{
    return ps_registry_lookup_(false, name);
}
