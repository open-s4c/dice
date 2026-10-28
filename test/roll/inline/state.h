#ifndef ROLL_INLINE_STATE_H
#define ROLL_INLINE_STATE_H
#include "dice.h"

struct state {
    unsigned order, serial, nested;
    bool stop, feedback, entered;
};

void publish_constant(struct state *state);
void publish_dynamic(struct state *state);
enum ps_err invoke_handler(struct state *state);
#endif
