#define DICE_MODULE_SLOT SLOT_APP
#include <assert.h>
#include <string.h>

#include "dice.h"

static ps_callback_f publish_;
static unsigned init_count_, ready_count_, event_count_;
static enum ps_err handler_result_ = PS_OK;

PS_SUBSCRIBE(CHAIN_CONTROL, EVENT_DICE_INIT, {
    init_count_++;
    /* Initialization dispatch has entered BLOCK; nested events must stop. */
    assert(publish_(CHAIN_FLOW, EVENT_TEST, NULL, NULL) == PS_STOP_CHAIN);
    assert(event_count_ == 0);
})

PS_SUBSCRIBE(CHAIN_CONTROL, EVENT_DICE_READY, {
    ready_count_++;
    assert(publish_(CHAIN_FLOW, EVENT_TEST, NULL, NULL) == PS_OK);
    assert(event_count_ == 1);
})

PS_SUBSCRIBE(CHAIN_FLOW, EVENT_TEST, {
    event_count_++;
    return handler_result_;
})

int
main(int argc, char **argv)
{
    assert(argc == 2);
    if (!strcmp(argv[1], "public"))
        publish_ = (ps_publish);
    else if (!strcmp(argv[1], "internal"))
        publish_ = dice_publish_internal_;
    else if (!strcmp(argv[1], "header"))
        publish_ = dice_publish_inline_;
    else {
        assert(!strcmp(argv[1], "nested"));
        publish_ = dice_publish_inline_nested_;
    }

    assert(publish_(CHAIN_FLOW, EVENT_TEST, NULL, NULL) == PS_OK);
    assert(init_count_ == 1 && ready_count_ == 1 && event_count_ == 2);
    assert(publish_(CHAIN_FLOW, EVENT_TEST, NULL, NULL) == PS_OK);
    assert(init_count_ == 1 && ready_count_ == 1 && event_count_ == 3);
    handler_result_ = PS_HANDLER_OFF;
    assert(publish_(CHAIN_FLOW, EVENT_TEST, NULL, NULL) == PS_OK);
    handler_result_ = PS_STOP_CHAIN;
    assert(publish_(CHAIN_FLOW, EVENT_TEST, NULL, NULL) == PS_STOP_CHAIN);
    assert(event_count_ == 5);
    return 0;
}
