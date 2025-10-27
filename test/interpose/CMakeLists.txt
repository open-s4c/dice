# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: 0BSD

set(INTERPOSED_IN ${CMAKE_CURRENT_SOURCE_DIR}/interposed.h.in)

set(FUNS_malloc malloc calloc realloc free posix_memalign aligned_alloc)
set(INCL_malloc '<dice/events/malloc.h>')

set(FUNS_memcpy memcpy)
set(INCL_memcpy '<dice/events/memcpy.h>')

set(FUNS_mman mmap munmap)
set(INCL_mman '<dice/events/mman.h>')

set(FUNS_pthread_create pthread_join)
set(INCL_pthread_create '<dice/events/pthread.h>')

set(FUNS_pthread_mutex pthread_mutex_lock pthread_mutex_unlock
                       pthread_mutex_trylock pthread_mutex_timedlock)
set(INCL_pthread_mutex ${INCL_pthread_create})

set(FUNS_pthread_cond pthread_cond_wait pthread_cond_timedwait
                      pthread_cond_signal pthread_cond_broadcast)
set(INCL_pthread_cond ${INCL_pthread_create})

set(FUNS_pthread_rwlock
    pthread_rwlock_rdlock
    pthread_rwlock_tryrdlock
    pthread_rwlock_timedrdlock
    pthread_rwlock_wrlock
    pthread_rwlock_trywrlock
    pthread_rwlock_timedwrlock
    pthread_rwlock_unlock)
set(INCL_pthread_rwlock ${INCL_pthread_create})

set(FUNS_annotate_rwlock AnnotateRWLockCreate AnnotateRWLockDestroy
                         AnnotateRWLockAcquired AnnotateRWLockReleased)
set(INCL_annotate_rwlock '<dice/events/annotate_rwlock.h>')

set(FUNS_sem sem_post sem_wait sem_trywait sem_timedwait)
set(INCL_sem '<dice/events/semaphore.h>')

set(FUNS_cxa __cxa_guard_acquire __cxa_guard_release __cxa_guard_abort)
set(INCL_cxa '<dice/events/cxa.h>')

set(MODS
    mman
    annotate_rwlock
    pthread_create
    pthread_mutex
    pthread_cond
    pthread_rwlock
    sem
    cxa)

if(NOT ${ENABLE_SANITIZER})
  # malloc does not play well with the sanitizers
  set(MODS malloc ${MODS})
endif()

# generate interpose test source
foreach(MOD ${MODS})
  set(TARGET ${MOD}_test)
  string(REPLACE ";" "\;" FUNS_${MOD} "${FUNS_${MOD}}")
  add_custom_command(
    OUTPUT ${TARGET}.c
    COMMAND
      cat ${INTERPOSED_IN} ${CMAKE_CURRENT_SOURCE_DIR}/interpose_test.c.in |
      $<TARGET_FILE:tmplr> -iP$$ "-DINCL=${INCL_${MOD}}" "-DFOO=${FUNS_${MOD}}"
      | $<TARGET_FILE:tmplr> -isP\\$$_ "-DFOO=${FUNS_${MOD}}" >
      ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}.c)

  add_custom_target(${MOD}-expand DEPENDS ${TARGET}.c)
  add_dependencies(expand-tests ${MOD}-expand)
endforeach()

# test cases
foreach(MOD ${MODS})
  set(TARGET ${MOD}_test)
  add_executable(${TARGET} ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}.c)
  add_test(NAME ${TARGET} COMMAND ${TARGET})
  target_link_libraries(${TARGET} PRIVATE dice.h dice.o tsano pthread)
  target_link_libraries(${TARGET} PRIVATE dice-${MOD}_testing.o)
  add_dependencies(${TARGET} ${MOD}-expand)

  if(${ENABLE_SANITIZER})
    target_compile_options(${TARGET} PRIVATE ${LIBSAN_C_FLAGS})
    target_link_options(${TARGET} PRIVATE ${LIBSAN_LD_FLAGS})
    target_link_libraries(${TARGET} PRIVATE ${LIBSAN_LINK})
  endif()
endforeach()

# Add the test executable
add_executable(memcpy_test ${CMAKE_CURRENT_SOURCE_DIR}/memcpy_test.c)

# Link any required libraries (optional)
target_link_libraries(memcpy_test PRIVATE dice.o tsano pthread)
add_test(NAME memcpy_test COMMAND memcpy_test)
