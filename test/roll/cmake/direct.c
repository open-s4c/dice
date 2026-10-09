#define DICE_MODULE_SLOT SLOT_SOURCE
#include "dice.h"

PS_SUBSCRIBE_SLOT(CHAIN_USER, EVENT_USER, SLOT_OBSERVER, { (*(int *)event)++; })

int
main(void)
{
    int count = 0;
    PS_PUBLISH(CHAIN_USER, EVENT_USER, &count, NULL);
    return count != 1;
}
