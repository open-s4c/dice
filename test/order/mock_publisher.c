#include "defs.h"

void
publish(struct event *ev)
{
    (void)ev;
    log_printf("Preload subscriber libraries!\n");
    exit(1);
}
