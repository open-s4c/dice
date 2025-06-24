# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

add_executable(rbtree_test rbtree_test.c rbtree.c)
target_link_libraries(rbtree_test PUBLIC dice.h)

add_test(NAME rbtree_test COMMAND rbtree_test)

# define event
execute_process(COMMAND seq 1 128 OUTPUT_VARIABLE EVENTS)
string(REGEX REPLACE "[ \r\n]+" ";" EVENTS ${EVENTS})
message(STATUS "EVENTS: ${EVENTS}")
set(EEE "")
foreach(EVENT ${EVENTS})
  string(CONCAT EEE "${EEE}${EVENT}\;")
endforeach()

# define slots
execute_process(COMMAND seq 0 15 OUTPUT_VARIABLE SLOTS)
string(REGEX REPLACE "[ \r\n]+" ";" SLOTS ${SLOTS})
message(STATUS "SLOTS: ${SLOTS}")
set(SSS "")
foreach(SLOT ${SLOTS})
  string(CONCAT SSS "${SSS}${SLOT}\;")
endforeach()

# define CHAINS
set(CHAINS
    0
    1
    2
    3
    4
    5
    6)
string(REGEX REPLACE ";" "," CCC "${CHAINS}")
string(REGEX REPLACE ";" "," SSX "${SLOTS}")
message(STATUS "CHAINS: ${CCC}")

add_custom_command(
  OUTPUT dispatch.c
  COMMAND $<TARGET_FILE:tmplr> "-DCCC=${CCC}" "-DSSS=${SSX}" -s dispatch.c.in >
          dispatch.c
  DEPENDS dispatch.c.in
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_target(update-dispatch DEPENDS dispatch.c)
set(DISPATCH_SRCS dispatch.c)

# CHAIN_CONTROL only has event 99
set(EEE_0 "99")
# INTERCEPT_EVENT and CAPTURE_EVENT only have THREAD_INIT, THREAD_FINI, ENTER
# and EXIT functions, and plain reads and writes. TODO: remove atomic reads and
# writes
set(EEE_1 "1;2;30;31;32;33;40;41")
set(EEE_4 "1;2;30;31;32;33;40;41")

foreach(CHAIN ${CHAINS})
  list(APPEND DISPATCH_SRCS dispatch_${CHAIN}.c)
  set(eee "${EEE}")
  if(EEE_${CHAIN})
    set(eee "${EEE_${CHAIN}}")
    string(REGEX REPLACE ";" "\\\;" eee "${eee}")
  endif()
  add_custom_command(
    OUTPUT dispatch_${CHAIN}.c
    COMMAND
      cat dispatch_chain.c.in | #
      $<TARGET_FILE:tmplr> "-DCCC=${CHAIN}" "-DEEE=${eee}" "-DSSS=${SSS}" -i |
      $<TARGET_FILE:tmplr> "-DEEE=${eee}" "-DSSS=${SSS}" -P_tmpl2 -i >
      dispatch_${CHAIN}.c
    DEPENDS dispatch_chain.c.in
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  add_custom_target(update-dispatch_${CHAIN} DEPENDS dispatch_${CHAIN}.c)
  add_dependencies(update-dispatch update-dispatch_${CHAIN})
endforeach()

set(SRCS)
foreach(SRC ${DISPATCH_SRCS})
  set(SRCS ${SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${SRC})
endforeach()

# dice libraries
set(SRCS mempool.c rbtree.c loader.c pubsub.c ${SRCS})
add_library(dice.o OBJECT ${SRCS})
target_link_libraries(dice.o PUBLIC dice.h)
target_compile_options(dice.o PRIVATE -fPIC -DDICE_BUILTIN)

add_library(dice-box.o OBJECT box.c)
target_link_libraries(dice-box.o PUBLIC dice.h)

add_library(dice SHARED ${SRCS})
target_link_libraries(dice PUBLIC dice.h pthread)
install(TARGETS dice DESTINATION lib)
