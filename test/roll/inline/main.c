#include <assert.h>

#include "state.h"

int
main(void)
{
    struct state first = {0}, second = {0}, stop = {.stop = true}, direct = {0};
    publish_constant(&first);
    assert(first.order == 12 && first.serial == 1 && first.nested == 1);
    publish_dynamic(&second);
    assert(second.order == 12 && second.serial == 2 && second.nested == 1);
    publish_constant(&stop);
    assert(stop.order == 1 && stop.serial == 3 && stop.nested == 0);
    assert(invoke_handler(&direct) == PS_OK);
    assert(direct.order == 1 && direct.serial == 4 && direct.nested == 1);
    struct state feedback = {.feedback = true};
    publish_constant(&feedback);
    assert(feedback.order == 122 && feedback.serial == 6 &&
           feedback.nested == 2 && !feedback.entered);
    return 0;
}
