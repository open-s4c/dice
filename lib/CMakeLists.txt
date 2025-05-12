# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

# add_library(bingo-core SHARED) target_link_libraries(bingo-core PRIVATE
# bingo-core.o bingo-base.o pthread) install(TARGETS bingo-core DESTINATION lib)

set(MODS
    self.o
    # pthread library
    pthread_create.o
    pthread_mutex.o
    pthread_rwlock.o
    pthread_cond.o
    # malloc free and friends
    malloc.o
    # semaphore
    sem.o
    # cxa guards
    cxa.o
    # tsan instrumented calls
    tsan.o
    stacktrace.o)

set(PRIO_self 201)

set(OBJS ${MODS} bingo-fastch.o)
foreach(TARGET ${OBJS})
  get_filename_component(MOD ${TARGET} NAME_WLE)
  if(PRIO_${MOD})
    target_compile_options(${TARGET} PRIVATE -DBINGO_XTOR_PRIO=${PRIO_${MOD}})
  endif()
  target_compile_options(${TARGET} PRIVATE -fPIC)
endforeach()

add_library(bingo SHARED)
target_link_libraries(bingo PUBLIC bingo.h)
target_link_libraries(bingo PUBLIC ${OBJS} bingo-core-plugin.o)

# for testing only
add_library(bingo-builtin SHARED)
target_link_libraries(bingo-builtin PUBLIC bingo.h)
target_link_libraries(bingo-builtin PUBLIC ${OBJS} bingo-core.o)

# install(TARGETS bingo DESTINATION lib) target_link_options(bingo PRIVATE
# -flto)
