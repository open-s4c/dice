# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: 0BSD

add_executable(rbtree_test rbtree_test.c rbtree.c)
target_link_libraries(rbtree_test PUBLIC dice.h)
add_test(NAME rbtree_test COMMAND rbtree_test)

# Create sequence of integers a 4th argument indicate the separator
function(seq OUT FROM TO)
  set(SEPARATOR " ")
  if(${ARGC} GREATER 3)
    set(SEPARATOR "${ARGV3}")
  endif()
  execute_process(COMMAND seq ${FROM} ${TO} OUTPUT_VARIABLE SEQUENCE)
  string(REGEX REPLACE "[ \r\n]+" " " SEQUENCE ${SEQUENCE})
  string(STRIP ${SEQUENCE} SEQUENCE)
  string(REPLACE " " "${SEPARATOR}" SEQUENCE ${SEQUENCE})
  set(${OUT}
      ${SEQUENCE}
      PARENT_SCOPE)
endfunction()

# define events, slots and chains
seq(EEE 1 128 \;)
seq(SSS 0 15 \;)
seq(SSX 0 15 ,)
seq(CHAINS 0 6 \;)
seq(CCC 0 6 ,)

add_custom_command(
  OUTPUT dispatch.c
  COMMAND $<TARGET_FILE:tmplr> "-DCCC=${CCC}" "-DSSS=${SSX}" -s dispatch.c.in >
          dispatch.c
  DEPENDS dispatch.c.in
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_target(expand-dispatch DEPENDS tmplr dispatch.c)

# CHAIN_CONTROL has only events 98..99
set(EEE_0 98 99)

set(DISPATCH_SRCS loader.c)
foreach(CHAIN ${CHAINS})
  list(APPEND DISPATCH_SRCS dispatch_${CHAIN}.c)
  set(eee "${EEE}")
  if(EEE_${CHAIN})
    set(eee "${EEE_${CHAIN}}")
  endif()
  add_custom_command(
    OUTPUT dispatch_${CHAIN}.c
    COMMAND
      cat dispatch_chain.c.in | #
      $<TARGET_FILE:tmplr> "-DCCC=${CHAIN}" "-DEEE='${eee}'" "-DSSS='${SSS}'" -i
      | $<TARGET_FILE:tmplr> "-DEEE='${eee}'" "-DSSS='${SSS}'" -P_tmpl2 -i >
      dispatch_${CHAIN}.c
    DEPENDS dispatch_chain.c.in
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  add_custom_target(expand-dispatch_${CHAIN} DEPENDS dispatch_${CHAIN}.c)
  add_dependencies(expand-dispatch expand-dispatch_${CHAIN})
endforeach()

set(_DSRCS)
foreach(SRC ${DISPATCH_SRCS})
  set(_DSRCS ${_DSRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${SRC})
endforeach()
set(DISPATCH_SRCS ${_DSRCS})

# dice libraries
set(SRCS mempool.c rbtree.c pubsub.c ${CMAKE_CURRENT_SOURCE_DIR}/dispatch.c)
add_library(dice.o OBJECT ${SRCS})
target_link_libraries(dice.o PUBLIC dice.h)
target_compile_options(dice.o PRIVATE -fPIC)

add_library(dice-dispatch.o OBJECT ${DISPATCH_SRCS})
target_compile_options(dice-dispatch.o PRIVATE -fPIC)
target_link_libraries(dice-dispatch.o PRIVATE dice.h)

# linking with dice-box.o will bring dice-dispatch.o as public dependency
add_library(dice-box.o OBJECT box.c)
target_compile_options(dice-box.o PRIVATE -fPIC)
target_link_libraries(dice-box.o PUBLIC dice-dispatch.o)
target_link_libraries(dice-box.o PRIVATE dice.h)

add_library(dice SHARED ${SRCS})
target_link_libraries(dice PUBLIC dice.h pthread)
install(TARGETS dice DESTINATION lib)

if(${DICE_COVERAGE})
  # do not involve dice-dispatch/dice-box in the coverage metrics
  set(TARGETS dice.o dice)
  foreach(TARGET ${TARGETS})
    target_compile_options(${TARGET} PRIVATE --coverage)
    target_link_options(${TARGET} PUBLIC --coverage)
  endforeach()
endif()

if(${ENABLE_SANITIZER})
  set(TARGETS rbtree_test dice.o dice)
  foreach(TARGET ${TARGETS})
    target_compile_options(${TARGET} PRIVATE ${LIBSAN_C_FLAGS})
    target_link_options(${TARGET} PUBLIC ${LIBSAN_LD_FLAGS})
    target_link_libraries(${TARGET} PUBLIC ${LIBSAN_LINK})
  endforeach()
endif()
