#ifndef DICE_PUBSUB_INTERNAL_H
#define DICE_PUBSUB_INTERNAL_H

#include <dice/pubsub.h>

struct sub {
    chain_id chain;
    chain_id type;
    ps_callback_f cb;
    int slot;
    struct sub *next;
};

struct type {
    size_t count;
    struct sub *head;
};

#ifdef DICE_ROLL_RUNTIME
DICE_HIDE int ps_subscribe_list_(struct type *, chain_id, type_id,
                                 ps_callback_f, int, bool);
DICE_HIDE enum ps_err ps_callback_invoke_(struct sub *, chain_id, type_id,
                                          void *, struct metadata *);

/* Interfaces shared by the core and configured dispatchers. */
DICE_HIDE void ps_init_(void);
DICE_HIDE enum ps_err ps_dispatch_(chain_id, type_id, void *,
                                   struct metadata *);
DICE_HIDE enum ps_err dice_publish_internal_(chain_id, type_id, void *,
                                             struct metadata *);
DICE_HIDE int ps_subscribe_(chain_id, type_id, ps_callback_f, int);
DICE_HIDE bool dice_chain_valid_(chain_id);
DICE_HIDE bool dice_event_valid_(type_id);
DICE_HIDE const char *dice_chain_name_(chain_id);
DICE_HIDE const char *dice_event_name_(type_id);
DICE_HIDE chain_id dice_chain_lookup_(const char *);
DICE_HIDE type_id dice_event_lookup_(const char *);
    #if DICE_EXTENSION_CALLBACKS
DICE_HIDE extern const type_id ps_event_ids_[];
DICE_HIDE extern const size_t ps_event_count_;
    #endif
    #if DICE_CALLBACKS
DICE_HIDE int dice_subscribe_known_(chain_id, type_id, ps_callback_f, int);
    #endif
    #if DICE_CHECK_ROUTES
DICE_HIDE bool dice_subscribes_allowed_(int, chain_id, type_id);
DICE_HIDE bool dice_publishes_allowed_(int, chain_id, type_id);
    #endif
#endif
#endif
