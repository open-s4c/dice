# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

add_subdirectory(core)
add_subdirectory(mod)
add_subdirectory(fastch)
add_subdirectory(tsano)

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

set(OBJS ${MODS} dice-fastch.o)
foreach(TARGET ${OBJS})
  get_filename_component(MOD ${TARGET} NAME_WLE)
  if(PRIO_${MOD})
    target_compile_options(${TARGET} PRIVATE -DDICE_XTOR_PRIO=${PRIO_${MOD}})
  endif()
  target_compile_options(${TARGET} PRIVATE -fPIC)
endforeach()

add_library(dice SHARED)
target_link_libraries(dice PUBLIC dice.h)
target_link_libraries(dice PUBLIC ${OBJS} dice-core-plugin.o)

# for testing only
add_library(dice-builtin SHARED)
target_link_libraries(dice-builtin PUBLIC dice.h)
target_link_libraries(dice-builtin PUBLIC ${OBJS} dice-core.o)

# install(TARGETS dice DESTINATION lib) target_link_options(dice PRIVATE
# -flto)
