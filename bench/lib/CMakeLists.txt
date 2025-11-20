# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

set(MODS
    dice-self.o
    dice-pthread_create.o
    dice-pthread_mutex.o
    dice-pthread_cond.o
    dice-pthread_rwlock.o
    dice-pthread_rwlock.o
    dice-tsan.o
    dice-stacktrace.o)

# dice-bundle - represents the scenario when building modules (dispatch-based)
# and plugin modules (callback-based) are required.
add_library(dice-bundle SHARED)
target_link_libraries(dice-bundle PRIVATE dice.o ${MODS})

# dice-bundle-box - represents the scenario when only building modules
# (dispatch-based) are required. It should perform faster than dice-bundle when
# compiled with LTO.
add_library(dice-bundle-box SHARED)
target_link_libraries(
  dice-bundle-box PRIVATE dice.o dice-pubsub-box.o dice-mempool-box.o
                          dice-self-box.o ${MODS})

# dice-bundle-cbonly - does not contain the dispatchers. It should only be used
# to measure the overhead of having callbacks and dispatchers in dice-bundle.
add_library(dice-bundle-cbonly SHARED)
target_link_libraries(dice-bundle-cbonly PRIVATE dice-cbonly.o ${MODS})
