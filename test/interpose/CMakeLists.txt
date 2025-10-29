# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

set(INCL_malloc '<dice/events/malloc.h>')
set(FUNS_malloc malloc calloc realloc free posix_memalign aligned_alloc)

set(INCL_memcpy '<dice/events/memcpy.h>')
set(FUNS_memcpy memcpy memmove memset)

set(INCL_mman '<dice/events/mman.h>')
set(FUNS_mman mmap munmap)

set(INCL_pthread_create '<dice/events/pthread.h>')
set(FUNS_pthread_create pthread_create pthread_join)

set(INCL_pthread_mutex ${INCL_pthread_create})
set(FUNS_pthread_mutex pthread_mutex_lock pthread_mutex_unlock
                       pthread_mutex_trylock)
if(NOT APPLE)
  set(FUNS_pthread_mutex ${FUNS_pthread_mutex} pthread_mutex_timedlock)
endif()

set(INCL_pthread_cond ${INCL_pthread_create})
set(FUNS_pthread_cond pthread_cond_wait pthread_cond_timedwait
                      pthread_cond_signal pthread_cond_broadcast)
if(NOT APPLE)
  set(FUNS_pthread_cond ${FUNS_pthread_cond} pthread_cond_timedwait)
endif()

set(INCL_pthread_rwlock ${INCL_pthread_create})
set(FUNS_pthread_rwlock
    pthread_rwlock_rdlock pthread_rwlock_tryrdlock pthread_rwlock_wrlock
    pthread_rwlock_trywrlock pthread_rwlock_unlock)

if(NOT APPLE)
  set(FUNS_pthread_rwlock ${FUNS_pthread_rwlock} pthread_rwlock_timedrdlock
                          pthread_rwlock_timedwrlock)
endif()

set(INCL_sem '<dice/events/semaphore.h>')
set(FUNS_sem sem_post sem_wait sem_trywait)
if(NOT APPLE)
  set(FUNS_sem ${FUNS_sem} sem_timedwait)
endif()

# TODO: create special tests for INTERCEPT_EVENT chain. Currently, we can only
# test INTERCEPT_BEFORE and INTERCEPT_AFTER.
set(INCL_annotate_rwlock '<dice/events/annotate_rwlock.h>')
set(FUNS_annotate_rwlock AnnotateRWLockCreate AnnotateRWLockDestroy
                         AnnotateRWLockAcquired AnnotateRWLockReleased)

set(FUNS_cxa __cxa_guard_acquire __cxa_guard_release __cxa_guard_abort)
set(INCL_cxa '<dice/events/cxa.h>')

set(MODS
    mman
    memcpy
    # annotate_rwlock: disabled until INTERCEPT_EVENT can be tested
    pthread_create
    pthread_mutex
    pthread_cond
    pthread_rwlock
    sem
    cxa)

if(${ENABLE_SANITIZER})
  message(STATUS "Ignoring malloc tests")
else()
  # malloc does not play well with the sanitizers
  set(MODS malloc ${MODS})
endif()

# generate interpose test source
foreach(MOD ${MODS})
  set(TARGET ${MOD}_test)
  set(TARGET_SRC ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.c)
  string(REPLACE ";" "\;" FUNS_${MOD} "${FUNS_${MOD}}")
  add_custom_command(
    OUTPUT ${TARGET_SRC}
    COMMAND
      $<TARGET_FILE:tmplr> -P$$ "-DINCL=${INCL_${MOD}}" "-DFOO=${FUNS_${MOD}}"
      ${CMAKE_CURRENT_SOURCE_DIR}/interposed_.h.in
      ${CMAKE_CURRENT_SOURCE_DIR}/interpose_test.c.in #
      > ${TARGET}_.c
    COMMAND
      $<TARGET_FILE:tmplr> -sP\\$$_ "-DFOO=${FUNS_${MOD}}"
      ${CMAKE_CURRENT_SOURCE_DIR}/interposed__.h.in ${TARGET}_.c #
      > ${TARGET}.c
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  add_custom_target(expand-test-${MOD} DEPENDS ${TARGET_SRC})
  add_dependencies(expand-tests expand-test-${MOD})
endforeach()

# test cases
foreach(MOD ${MODS})
  set(TARGET ${MOD}_test)
  add_executable(${TARGET} ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.c)
  target_link_libraries(${TARGET} PRIVATE dice.h dice tsano pthread)
  target_compile_options(
    ${TARGET} PRIVATE -fno-builtin-memcpy -fno-builtin-memset
                      -fno-builtin-memmmove)
  if(APPLE)
    # We keep the expanded versions of the test cases in the repo. However, when
    # running on macOS, we regerate the test files without timed variants, which
    # are not available in that system.
    add_dependencies(${TARGET} expand-test-${MOD})
  endif()

  set(PRELOAD $<TARGET_FILE:dice-${MOD}>)

  if(${MOD} STREQUAL "memcpy")
    set(PRELOAD "$<TARGET_FILE:dice-wrap_memset>:${PRELOAD}")
  endif()

  add_test(NAME interpose-${TARGET}
           COMMAND ${DICE_SCRIPT} -preload ${PRELOAD}
                   ${CMAKE_CURRENT_BINARY_DIR}/${TARGET})

  if(${ENABLE_SANITIZER})
    target_compile_options(${TARGET} PRIVATE ${LIBSAN_C_FLAGS})
    target_link_options(${TARGET} PRIVATE ${LIBSAN_LD_FLAGS})
    target_link_libraries(${TARGET} PRIVATE ${LIBSAN_LINK})
  endif()
endforeach()

# Add the test executable for memcpy
if(NOT APPLE)
  add_executable(memcpy_test ${CMAKE_CURRENT_SOURCE_DIR}/memcpy_test.c)
  target_link_libraries(memcpy_test PRIVATE dice.o tsano pthread)
  add_test(NAME interpose-memcpy_test COMMAND memcpy_test)
endif()

target_link_libraries(cxa_test PRIVATE stdc++)
