# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

add_executable(micro micro.c)
target_link_libraries(micro dice pthread)

add_executable(micro2 micro2.c)
target_link_libraries(micro2 dice pthread)

add_library(microcb.o OBJECT microcb.c)
target_link_libraries(microcb.o dice.h)

set(MODS
    microcb.o
    mod-self.o
    # pthread library
    mod-pthread_create.o
    mod-pthread_mutex.o
    mod-pthread_rwlock.o
    mod-pthread_cond.o
    # malloc free and friends
    mod-malloc.o
    # semaphore
    mod-sem.o
    # cxa guards
    mod-cxa.o
    # tsan instrumented calls
    mod-tsan.o
    mod-stacktrace.o)

set(PRIO_mod-self 4)
set(PRIO_mod-microcb 5)

set(OBJS ${MODS} dice.o)
foreach(TARGET ${OBJS})
  get_filename_component(MOD ${TARGET} NAME_WLE)
  if(PRIO_${MOD})
    target_compile_options(${TARGET} PRIVATE -DDICE_MODULE_PRIO=${PRIO_${MOD}})
  endif()
  target_compile_options(${TARGET} PRIVATE -fPIC)
endforeach()

add_library(micro-dice SHARED)
target_link_libraries(micro-dice PUBLIC ${OBJS} dice-box.o)

add_executable(micro3 micro3.c)
target_link_libraries(micro3 PUBLIC micro-dice)
