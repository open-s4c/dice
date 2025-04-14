add_executable(rbtree_test rbtree_test.c rbtree.c)

set(SRCS
    # Stateless components
    uset.c
    idmap.c
    rbtree.c
    # Stateful components
    mempool.c
    intercept.c
    # pubsub component must be the last core component to be initialized. it
    # guarantees no event is published before the previous components are ready.
    pubsub.c
    # the first module subscribing to all events from pubsub. It implements TLS
    # management with the memory from mempool. It blocks any messages of a
    # thread before they publish an TASK_INIT event. It blocks any messages sent
    # by a thread after it sends a TASK_FINI event.
    self.c
    # switcher is an optional module. It does not influence the execution unless
    # it is actively used by some module.
    switcher.c)

add_library(bingo-core SHARED ${SRCS})
target_link_libraries(bingo-core PUBLIC bingo.h)
target_link_libraries(bingo-core PRIVATE pthread)
install(TARGETS bingo-core DESTINATION lib)

set(MODS
    # pthread library
    ../mod/pthread_create.c
    ../mod/pthread_mutex.c
    ../mod/pthread_cond.c
    # malloc free and friends
    ../mod/malloc.c
    # semaphore
    ../mod/sem.c
    # cxa guards
    ../mod/cxa.cpp
    # tsan and tsan func-entry/exit
    ../mod/tsan.c
    ../mod/stacktrace.c)

add_library(bingo.a STATIC ${SRCS} ${MODS})
target_link_libraries(bingo.a PUBLIC bingo.h)
set_property(TARGET bingo.a PROPERTY SUFFIX "")
install(TARGETS bingo.a DESTINATION lib/bingo)

add_library(bingo SHARED ${SRCS} ${MODS})
target_link_libraries(bingo PUBLIC bingo.h)
target_link_libraries(bingo PRIVATE pthread)
install(TARGETS bingo DESTINATION lib)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  set_property(TARGET bingo PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  set_property(TARGET bingo.a PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  target_compile_options(bingo PRIVATE -fPIC -flto -ffat-lto-objects)
  target_compile_options(bingo.a PRIVATE -fPIC -flto -ffat-lto-objects)
endif()
