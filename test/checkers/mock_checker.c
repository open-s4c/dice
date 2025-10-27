/* this is a mock checker used to link with test cases that need to call the
 * registration function(s).  */

#include "trace_checker.h"

DICE_WEAK void
register_expected_trace(thread_id id, struct expected_event *trace)
{
    (void)id;
    (void)trace;
}
