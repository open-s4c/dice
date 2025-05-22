# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

add_executable(micro micro.c)
target_link_libraries(micro dice pthread)

add_executable(micro2 micro2.c)
target_link_libraries(micro2 dice pthread)

add_library(microcb.o OBJECT microcb.c)
target_link_libraries(microcb.o dice.h)

set(MODS
    self.o
    microcb.o
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
set(PRIO_microcb 202)

set(OBJS ${MODS} dice-fastch.o dice-core.o)
foreach(TARGET ${OBJS})
  get_filename_component(MOD ${TARGET} NAME_WLE)
  if(PRIO_${MOD})
    target_compile_options(${TARGET} PRIVATE -DDICE_XTOR_PRIO=${PRIO_${MOD}})
  endif()
  target_compile_options(${TARGET} PRIVATE -fPIC)
endforeach()

add_library(micro-dice SHARED)
target_link_libraries(micro-dice PUBLIC ${OBJS})

add_executable(micro3 micro3.c)
target_link_libraries(micro3 micro-dice pthread)
