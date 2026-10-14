#ifndef COMPAT_EVENTS_H
#define COMPAT_EVENTS_H

#ifndef DICE_ROLLED
    #define EVENT_COMPAT 200
    #define CHAIN_COMPAT 11
#endif

struct compat_event {
    int count;
};
#endif
