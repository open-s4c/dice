# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: 0BSD

add_executable(micro micro.c)
target_link_libraries(micro dice pthread)

add_executable(micro2 micro2.c)
target_link_libraries(micro2 dice pthread)

add_library(microcb.o OBJECT microcb.c)
target_link_libraries(microcb.o dice.h)

add_library(microcb-tls.o OBJECT microcb-tls.c)
target_link_libraries(microcb-tls.o dice.h)

add_library(microcb-multi.o OBJECT microcb-multi.c)
target_link_libraries(microcb-multi.o dice.h)

set(MODS
    dice.o
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

set(SLOT_dice-self 4)
set(SLOT_microcb 5)
set(SLOT_microcb-tls 5)
set(SLOT_microcb-multi 5)

set(OBJS ${MODS} microcb.o microcb-tls.o microcb-multi.o)
foreach(TARGET ${OBJS})
  get_filename_component(MOD ${TARGET} NAME_WLE)
  if(SLOT_${MOD})
    target_compile_options(${TARGET} PRIVATE -DDICE_MODULE_SLOT=${SLOT_${MOD}})
  endif()
  target_compile_options(${TARGET} PRIVATE -fPIC)
endforeach()

add_library(micro-dice SHARED)
target_link_libraries(micro-dice PUBLIC ${MODS} microcb.o dice-box.o pthread)

add_library(micro-dice-tls SHARED)
target_link_libraries(micro-dice-tls PUBLIC ${MODS} microcb-tls.o dice-box.o
                                            pthread)
add_library(micro-dice-multi SHARED)
target_link_libraries(micro-dice-multi PUBLIC ${MODS} microcb-multi.o
                                              dice-box.o pthread)
if(APPLE)
  target_link_libraries(micro-dice PRIVATE stdc++)
  target_link_libraries(micro-dice-tls PRIVATE stdc++)
  target_link_libraries(micro-dice-multi PRIVATE stdc++)
endif()

if(${ENABLE_SANITIZER})
  target_link_options(micro-dice PRIVATE ${LIBSAN_SHARED})
  target_link_options(micro-dice-tls PRIVATE ${LIBSAN_SHARED})
  target_link_options(micro-dice-multi PRIVATE ${LIBSAN_SHARED})
endif()

add_executable(micro3 micro3.c)
target_link_libraries(micro3 PUBLIC micro-dice)

add_executable(micro4 micro4.c)
target_link_libraries(micro4 PUBLIC micro-dice-tls)

add_executable(micro5 micro5.c)
target_link_libraries(micro5 PUBLIC micro-dice-multi)
