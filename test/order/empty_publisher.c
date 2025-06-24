#include "defs.h"

DICE_WEAK void
publish(struct event *ev)
{
    (void)ev;
    log_printf("Preload subscriber libraries!\n");
    exit(1);
}
