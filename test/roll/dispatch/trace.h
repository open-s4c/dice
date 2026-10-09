#ifndef ROLL_TRACE_H
#define ROLL_TRACE_H
#include "dice.h"

struct trace {
    char text[64];
    unsigned length;
    char stop;
};

static inline enum ps_err
record(void *event, char name)
{
    struct trace *trace          = event;
    trace->text[trace->length++] = name;
    trace->text[trace->length]   = 0;
    return trace->stop == name ? PS_STOP_CHAIN : PS_OK;
}
#endif
