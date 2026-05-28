# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

set(MODS)

# ------------------------------------------------------------------------------
# malloc, mman and friends
# ------------------------------------------------------------------------------

if(${ENABLE_SANITIZER})
  # malloc does not play well with the sanitizers
  message(STATUS "Ignoring malloc tests")
else()
  list(APPEND MODS malloc)
  set(INCL_malloc '<dice/events/malloc.h>')
  set(FUNS_malloc malloc calloc realloc free posix_memalign aligned_alloc)

  if(NOT APPLE)
    list(APPEND MODS memcpy)
    set(INCL_memcpy '<dice/events/memcpy.h>')
    set(FUNS_memcpy memcpy memmove)
    if(${DICE_MEMSET})
      set(FUNS_memcpy ${FUNS_memcpy} memset)
    endif()
  endif()

  list(APPEND MODS mman)
  set(INCL_mman '<dice/events/mman.h>')
  set(FUNS_mman mmap munmap)
  if(NOT APPLE)
    list(APPEND FUNCS_mman mmap64)
  endif()
endif()

# ------------------------------------------------------------------------------
# pthreads
# ------------------------------------------------------------------------------

list(APPEND MODS pthread_create)
set(INCL_pthread_create '<dice/events/pthread.h>')
set(FUNS_pthread_create pthread_create pthread_join)

list(APPEND MODS pthread_mutex)
set(INCL_pthread_mutex ${INCL_pthread_create})
set(FUNS_pthread_mutex pthread_mutex_lock pthread_mutex_unlock
                       pthread_mutex_trylock)

if(HAVE_PTHREAD_CLOCKLOCK_CLOCKWAIT)
  set(FUNS_pthread_mutex ${FUNS_pthread_mutex} pthread_mutex_clocklock)
endif()

if(NOT APPLE)
  set(FUNS_pthread_mutex ${FUNS_pthread_mutex} pthread_mutex_timedlock)
endif()

list(APPEND MODS pthread_cond)
set(INCL_pthread_cond ${INCL_pthread_create})
set(FUNS_pthread_cond pthread_cond_wait pthread_cond_timedwait
                      pthread_cond_signal pthread_cond_broadcast)

if(HAVE_PTHREAD_CLOCKLOCK_CLOCKWAIT)
  set(FUNS_pthread_cond ${FUNS_pthread_cond} pthread_cond_clockwait)
endif()

list(APPEND MODS pthread_rwlock)
set(INCL_pthread_rwlock ${INCL_pthread_create})
set(FUNS_pthread_rwlock
    pthread_rwlock_rdlock pthread_rwlock_tryrdlock pthread_rwlock_wrlock
    pthread_rwlock_trywrlock pthread_rwlock_unlock)

if(NOT APPLE)
  list(APPEND MODS pthread_spinlock)
  set(FUNS_pthread_rwlock ${FUNS_pthread_rwlock} pthread_rwlock_timedrdlock
                          pthread_rwlock_timedwrlock)
  set(INCL_pthread_spinlock ${INCL_pthread_create})
  set(FUNS_pthread_spinlock ${FUNS_pthread_spinlock} pthread_spin_lock
                            pthread_spin_trylock pthread_spin_unlock)
endif()

# ------------------------------------------------------------------------------
# semaphore
# ------------------------------------------------------------------------------

list(APPEND MODS sem)
set(INCL_sem '<dice/events/semaphore.h>')
set(FUNS_sem sem_post sem_wait sem_trywait)
if(NOT APPLE)
  set(FUNS_sem ${FUNS_sem} sem_timedwait)
endif()

# ------------------------------------------------------------------------------
# random
# ------------------------------------------------------------------------------

if(NOT APPLE)
  list(APPEND MODS random)
  set(INCL_random '<dice/events/random.h>')
  set(FUNS_random getrandom)
endif()

# ------------------------------------------------------------------------------
# poll, epoll, and other fd related
# ------------------------------------------------------------------------------

list(APPEND MODS poll)
set(INCL_poll '<dice/events/poll.h>')
set(FUNS_poll poll)

if(NOT APPLE AND NOT BSD)
  list(APPEND MODS epoll)
endif()
set(INCL_epoll '<dice/events/epoll.h>')
set(FUNS_epoll epoll_create epoll_create1 epoll_ctl epoll_wait epoll_pwait)

if(HAVE_EPOLL_PWAIT2 AND NOT MUSL)
  list(APPEND FUNS_epoll epoll_pwait2)
endif()

if(NOT APPLE)
  list(APPEND MODS eventfd)
  set(INCL_eventfd '<dice/events/eventfd.h>')
  set(FUNS_eventfd eventfd)
endif()

if(NOT APPLE)
  list(APPEND MODS fcntl)
endif()
set(INCL_fcntl '<dice/events/fcntl.h>')
set(FUNS_fcntl creat #[[fcntl]] #[[openat]] posix_fadvise posix_fallocate)
if (NOT (CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_BUILD_TYPE STREQUAL "Release"))
  list(APPEND FUNS_fcntl open)
endif()

# ------------------------------------------------------------------------------
# socket related
# ------------------------------------------------------------------------------

list(APPEND MODS socket)
set(INCL_socket '<dice/events/socket.h>')
set(FUNS_socket
    accept
    bind
    connect
    getpeername
    getsockname
    getsockopt
    setsockopt
    listen
    recv
    recvfrom
    recvmsg
    send
    sendto
    sendmsg
    shutdown
    socket
    socketpair)
if(NOT APPLE)
  list(APPEND FUNS_socket accept4)
endif()

# ------------------------------------------------------------------------------
# time.h
# ------------------------------------------------------------------------------
if (NOT (CMAKE_C_COMPILER_ID STREQUAL "Clang" AND CMAKE_BUILD_TYPE STREQUAL "Release") AND NOT BSD)
  list(APPEND MODS time)
endif()
set(INCL_time '<dice/events/time.h>')
set(FUNS_time
    asctime
    asctime_r
    clock
    time
    clock_getres
    clock_gettime
    clock_settime
    ctime
    ctime_r
    difftime
    gmtime
    gmtime_r
    localtime
    localtime_r
    mktime
    nanosleep
    strftime
    strptime
    getitimer
    setitimer
    gettimeofday
    select
    tzset
    utimes)

if (HAVE_GETDATE AND NOT BSD)
  list(APPEND FUNS_time getdate)
endif()

if (HAVE_SETTIMEOFDAY AND NOT BSD)
  list(APPEND FUNS_time settimeofday)
endif()

if(NOT APPLE)
  list(
    APPEND
    FUNS_time
    timer_create
    timer_delete
    timer_gettime
    timer_getoverrun
    timer_settime
    strftime_l)
endif()

if (HAVE_GETDATE_R)
  list(APPEND FUNS_time getdate_r)
endif()

# ------------------------------------------------------------------------------
# uio.h
# ------------------------------------------------------------------------------

if(NOT APPLE AND NOT BSD)
  list(APPEND MODS uio)
  set(INCL_uio '<dice/events/uio.h>')
  set(FUNS_uio readv writev)
endif()

# ------------------------------------------------------------------------------
# unistd.h
# ------------------------------------------------------------------------------
if (NOT (CMAKE_C_COMPILER_ID STREQUAL "Clang" AND CMAKE_BUILD_TYPE STREQUAL "Release"))
  list(APPEND MODS unistd)
endif()
set(INCL_unistd '<dice/events/unistd.h>')
set(FUNS_unistd
    getdtablesize
    getegid
    geteuid
    write
    getgid
    getgroups
    getlogin
    getpagesize
    getpgrp
    getpid
    getppid
    getuid
    fork
    pause
    #pipe
    setsid
    sync
    access
    alarm
    chdir
    chown
    chroot
    close
    confstr
    ctermid
    dup
    dup2
    faccessat
    fchdir
    fchown
    fchownat
    fpathconf
    fsync
    ftruncate
    getcwd
    getlogin_r
    getopt
    getpgid
    getsid
    isatty
    lchown
    link
    linkat
    lockf
    lseek
    nice
    pathconf
    pread
    pwrite
    read
    readlink
    readlinkat
    rmdir
    setgid
    setpgid
    setregid
    setreuid
    setuid
    sleep
    swab
    symlink
    symlinkat
    sysconf
    tcgetpgrp
    tcsetpgrp
    truncate
    ttyname
    ttyname_r
    unlink
    unlinkat
    usleep)

if(NOT APPLE)
  list(
    APPEND
    FUNS_unistd
    brk
    crypt
    fdatasync
    #pipe2
    sbrk
    vfork)
endif()

if(NOT APPLE AND NOT BSD)
  list(APPEND FUNS_unistd setpgrp syncfs dup3)
endif()

if(HAVE_GETHOSTID)
  list(APPEND FUNS_unistd gethostid)
endif()

if(HAVE_SETHOSTID AND NOT APPLE AND NOT BSD)
  list(APPEND FUNS_unistd sethostid)
endif()

if(HAVE_GETPASS)
  list(APPEND FUNS_unistd getpass)
endif()

if(HAVE_GETWD)
  list(APPEND FUNS_unistd getwd)
endif()

if(HAVE_GET_CURRENT_DIR_NAME)
  list(APPEND FUNS_unistd get_current_dir_name)
endif()

if(HAVE_CUSERID)
  list(APPEND FUNS_unistd cuserid)
endif()

if(HAVE_UALARM)
  list(APPEND FUNS_unistd ualarm)
endif()

# ------------------------------------------------------------------------------
# Sanitizer's annotate_rwlock
# ------------------------------------------------------------------------------
# TODO: create special tests for INTERCEPT_EVENT chain. Currently, we can only
# test INTERCEPT_BEFORE and INTERCEPT_AFTER.

#[[list(APPEND MODS annotate_rwlock)]]
set(INCL_annotate_rwlock '<dice/events/annotate_rwlock.h>')
set(FUNS_annotate_rwlock AnnotateRWLockCreate AnnotateRWLockDestroy
                         AnnotateRWLockAcquired AnnotateRWLockReleased)

# ------------------------------------------------------------------------------
# C++'s __cxa_guard
# ------------------------------------------------------------------------------

list(APPEND MODS cxa)
set(FUNS_cxa __cxa_guard_acquire __cxa_guard_release __cxa_guard_abort)
set(INCL_cxa '<dice/events/cxa.h>')

# ------------------------------------------------------------------------------
# Generators and tests
# ------------------------------------------------------------------------------

# generate interpose test source
foreach(MOD ${MODS})
  set(TARGET ${MOD}_test)
  set(TARGET_SRC ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.c)
  string(REPLACE ";" "\;" FUNS_${MOD} "${FUNS_${MOD}}")
  add_custom_command(
    OUTPUT ${TARGET_SRC}
    COMMAND
      $<TARGET_FILE:tmplr> -l 1024 "-DINCL=${INCL_${MOD}}"
      "-DFOO=${FUNS_${MOD}}" ${CMAKE_CURRENT_SOURCE_DIR}/interposed_.h.in
      ${CMAKE_CURRENT_SOURCE_DIR}/interpose_test.c.in #
      > ${TARGET}_.c.tmp
    COMMAND ${CMAKE_COMMAND} -E rename ${TARGET}_.c.tmp ${TARGET}_.c
    COMMAND
      $<TARGET_FILE:tmplr> -l 1024 -sP\\$$_ "-DFOO=${FUNS_${MOD}}"
      ${CMAKE_CURRENT_SOURCE_DIR}/interposed__.h.in ${TARGET}_.c #
      > ${TARGET}.c.tmp
    COMMAND ${CMAKE_COMMAND} -E rename ${TARGET}.c.tmp ${TARGET}.c
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/interpose_test.c.in
            ${CMAKE_CURRENT_SOURCE_DIR}/interposed_.h.in
            ${CMAKE_CURRENT_SOURCE_DIR}/interposed__.h.in)
  add_custom_target(expand-test-${MOD} DEPENDS ${TARGET_SRC})
  set_source_files_properties(${TARGET_SRC} PROPERTIES GENERATED TRUE)
  add_dependencies(expand-tests expand-test-${MOD})
endforeach()

# test cases
foreach(MOD ${MODS})
  set(TARGET ${MOD}_test)
  add_executable(${TARGET} ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.c)
  add_dependencies(${TARGET} expand-test-${MOD})
  target_link_libraries(${TARGET} PRIVATE dice.h dice tsano pthread)
  if (NOT APPLE)
     target_link_libraries(${TARGET} PRIVATE crypt)
  endif()
  target_compile_options(
    ${TARGET} PRIVATE -fno-builtin-memcpy -fno-builtin-memset
                      -fno-builtin-memmmove -Wno-deprecated-declarations)

  set(PRELOAD $<TARGET_FILE:dice-${MOD}>)

  if(${MOD} STREQUAL "memcpy" AND ${DICE_MEMSET})
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

target_link_libraries(cxa_test PRIVATE stdc++)
