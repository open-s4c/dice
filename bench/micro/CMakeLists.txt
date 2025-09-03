# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: 0BSD

add_executable(micro micro.c)
target_link_libraries(micro dice pthread)

add_executable(micro2 micro2.c)
target_link_libraries(micro2 dice pthread)

add_library(microcb.o OBJECT microcb.c)
target_link_libraries(microcb.o dice.h)

set(MODS
    microcb.o
    dice-self.o
    # pthread library
    dice-pthread_create.o
    dice-pthread_mutex.o
    dice-pthread_rwlock.o
    dice-pthread_cond.o
    # malloc free and friends
    dice-malloc.o
    # semaphore
    dice-sem.o
    # cxa guards
    dice-cxa.o
    # tsan instrumented calls
    dice-tsan.o
    dice-stacktrace.o)

set(PRIO_dice-self 4)
set(PRIO_dice-microcb 5)

set(OBJS ${MODS} dice.o)
foreach(TARGET ${OBJS})
  get_filename_component(MOD ${TARGET} NAME_WLE)
  if(PRIO_${MOD})
    target_compile_options(${TARGET} PRIVATE -DDICE_MODULE_PRIO=${PRIO_${MOD}})
  endif()
  target_compile_options(${TARGET} PRIVATE -fPIC)
endforeach()

add_library(micro-dice SHARED)
target_link_libraries(micro-dice PUBLIC ${OBJS} dice-box.o pthread)
if(${ENABLE_SANITIZER})
  target_link_options(micro-dice PRIVATE ${LIBSAN_SHARED})
endif()

add_executable(micro3 micro3.c)
target_link_libraries(micro3 PUBLIC micro-dice)
