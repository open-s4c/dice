# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

add_executable(rbtree_test rbtree_test.c rbtree.c)
target_link_libraries(rbtree_test PUBLIC dice.h)
add_test(NAME rbtree_test COMMAND rbtree_test)

# ------------------------------------------------------------------------------
# dispatchers generation
# ------------------------------------------------------------------------------
# Create sequence of integers. The 4th argument indicates the separator
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
# Manually create the sequencer of chains to ensure 1 and 4 are in the front.
# That improves the chances of LTO optimizing them.
set(CCC "1,4,2,3,5,6,0")

set(DISPATCH_C ${CMAKE_CURRENT_BINARY_DIR}/dispatch.c)

add_custom_command(
  OUTPUT ${DISPATCH_C}
  COMMAND $<TARGET_FILE:tmplr> "-DCCC=${CCC}" "-DSSS=${SSX}" -s
          ${CMAKE_CURRENT_SOURCE_DIR}/dispatch.c.in > dispatch.c
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dispatch.c.in
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(expand-dispatch DEPENDS tmplr ${DISPATCH_C})

# CHAIN_CONTROL has only events 98..99
set(EEE_0 98 99)

set(DISPATCH_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/loader.c)
set(DISPATCH_GENERATED ${DISPATCH_C})
foreach(CHAIN ${CHAINS})
  set(DISPATCH_CHAIN_C ${CMAKE_CURRENT_BINARY_DIR}/dispatch_${CHAIN}.c)
  list(APPEND DISPATCH_GENERATED ${DISPATCH_CHAIN_C})
  set(eee "${EEE}")
  if(EEE_${CHAIN})
    set(eee "${EEE_${CHAIN}}")
  endif()
  add_custom_command(
    OUTPUT ${DISPATCH_CHAIN_C}
    COMMAND
      cat ${CMAKE_CURRENT_SOURCE_DIR}/dispatch_chain.c.in | #
      $<TARGET_FILE:tmplr> "-DCCC=${CHAIN}" "-DEEE='${eee}'" "-DSSS='${SSS}'" -i
      | $<TARGET_FILE:tmplr> "-DEEE='${eee}'" "-DSSS='${SSS}'" -P_tmpl2 -i >
      dispatch_${CHAIN}.c
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dispatch_chain.c.in
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  add_custom_target(expand-dispatch_${CHAIN} DEPENDS ${DISPATCH_CHAIN_C})
  add_dependencies(expand-dispatch expand-dispatch_${CHAIN})
endforeach()
set(DISPATCH_SRCS ${DISPATCH_SRCS} ${DISPATCH_GENERATED})

# ------------------------------------------------------------------------------
# dice libraries
# ------------------------------------------------------------------------------
set(SRCS mempool.c rbtree.c pubsub.c registry.c memset.c ${DISPATCH_C})

# dice (libdice.so) - SHARED library providing interface for plugin modules.
# Only contains the core components mempool and pubsub. This library gets
# installed in the system. Dispatch won't be used for this library except for
# the internal initialization with chain 0 (CHAIN_CONTROL).
add_library(dice SHARED ${SRCS})
target_link_libraries(dice PUBLIC dice.h pthread)
install(TARGETS dice DESTINATION lib)

# dice.o - OBJECT library providing interface for plugin modules and builtin
# modules. Only contains the core components mempool, pubsub, and dispatch, but
# can be linked to other modules. See the dispatcher generation above for
# details on the chains, events and slots handled by dispatch and callbacks.
add_library(dice.o OBJECT ${SRCS} ${DISPATCH_SRCS})
target_link_libraries(dice.o PUBLIC dice.h)
target_compile_options(dice.o PRIVATE -fPIC)

# dice-box.o - OBJECT library that seals pubsub, registry, rbtree, and mempool
# interfaces, disabling callbacks when linked with dice.o. This allows the LTO
# steps to be more aggressive.
add_library(dice-box.o OBJECT pubsub-box.c mempool-box.c rbtree.c)
target_compile_definitions(dice-box.o PRIVATE DICE_HIDE_ALL)
target_compile_options(dice-box.o PRIVATE -fPIC)
target_link_libraries(dice-box.o PRIVATE dice.h)

# dice-mempool-box.o - OBJECT library that seals mempool interface.
add_library(dice-mempool-box.o OBJECT mempool-box.c)
target_compile_options(dice-mempool-box.o PRIVATE -fPIC)
target_link_libraries(dice-mempool-box.o PRIVATE dice.h)

# dice-pubsub-box.o - OBJECT library that seals pubsub, registry and rbtree
# interfaces, disabling callbacks when linked with dice.o.
add_library(dice-pubsub-box.o OBJECT pubsub-box.c rbtree.c)
target_compile_definitions(dice-pubsub-box.o PRIVATE DICE_HIDE_ALL)
target_compile_options(dice-pubsub-box.o PRIVATE -fPIC)
target_link_libraries(dice-pubsub-box.o PRIVATE dice.h)

# dice-cbonly.o - OBJECT library as dice.o but without dispatchers. Only used
# for performance testing.
add_library(dice-cbonly.o OBJECT ${SRCS})
target_link_libraries(dice-cbonly.o PUBLIC dice.h)
target_compile_options(dice-cbonly.o PRIVATE -fPIC)

# dice-override.o - OBJECT library to override memset (and potentially others)
add_library(dice-override.o OBJECT memset.c)
target_link_libraries(dice-override.o PUBLIC dice.h)
target_compile_options(dice-override.o PRIVATE -fPIC)

# ------------------------------------------------------------------------------
# test configuration
# ------------------------------------------------------------------------------

if(${DICE_COVERAGE})
  # do not involve dice-box in the coverage metrics
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
