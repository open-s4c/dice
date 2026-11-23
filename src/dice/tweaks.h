#ifndef DICE_TWEAKS_H
#define DICE_TWEAKS_H
#include <dice/compiler.h>

#if defined(__clang__) || defined(DICE_ROLL_RUNTIME)
// This variable should be a static variable inside ps_initd_. With a good
// LTO, the check of this variable should be done by the caller before
// calling the function. However, in versions of clang (14 to 19), the LTO
// linker is not clever enough to move the check to the caller, so we do it
// manually.
// Rolled header dispatch needs the same check with GCC, which can otherwise
// leave a call to the complete initializer on every publication.

DICE_WEAK DICE_HIDE bool ready_;

    #define PS_NOT_INITD_() (unlikely(!ready_) && !ps_initd_())
#else
    #define PS_NOT_INITD_() (unlikely(!ps_initd_()))
#endif


#endif // DICE_TWEAKS_H
