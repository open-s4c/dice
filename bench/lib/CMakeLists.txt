# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

set(MODS
    self
    pthread_create
    pthread_mutex
    pthread_cond
    pthread_rwlock
    tsan
    stacktrace)

# dice-bundle - represents the scenario when building modules (dispatch-based)
# and plugin modules (callback-based) are required.
add_library(dice-bundle SHARED)
add_dice_core(dice-bundle bundle.dice)
add_dice_mods(dice-bundle ${MODS})

# dice-bundle-box - represents the scenario when only building modules
# (dispatch-based) are required. It should perform faster than dice-bundle when
# compiled with LTO.
add_library(dice-bundle-box SHARED)
add_dice_core(dice-bundle-box box.dice)
add_dice_mods(dice-bundle-box ${MODS})
# These visibility overrides contain no slot/event references. Preserve the
# existing benchmark's allocator and self API sealing in addition to pubsub.
target_link_libraries(dice-bundle-box PRIVATE dice-mempool-box.o
                                              dice-self-box.o)

# dice-bundle-cbonly - does not contain the dispatchers. It should only be used
# to measure the overhead of having callbacks and dispatchers in dice-bundle.
add_library(dice-bundle-cbonly SHARED)
# Keep the legacy callback-only control: generated known slots use dispatch.
set(CALLBACK_MODS)
foreach(module IN LISTS MODS)
  list(APPEND CALLBACK_MODS "dice-${module}.o")
endforeach()
target_link_libraries(dice-bundle-cbonly PRIVATE dice-core.o ${CALLBACK_MODS})
