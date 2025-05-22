# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

# define CHAINS
execute_process(COMMAND seq 1 6 OUTPUT_VARIABLE CHAINS)
string(REGEX REPLACE "[ \r\n]+" ";" CHAINS ${CHAINS})
message(STATUS "CHAINS: ${CHAINS}")
set(CCC "")
foreach(CHAIN ${CHAINS})
  string(CONCAT CCC "${CCC}${CHAIN}\;")
endforeach()

# define event
execute_process(COMMAND seq 1 100 OUTPUT_VARIABLE EVENTS)
string(REGEX REPLACE "[ \r\n]+" ";" EVENTS ${EVENTS})
message(STATUS "EVENTS: ${EVENTS}")
set(EEE "")
foreach(EVENT ${EVENTS})
  string(CONCAT EEE "${EEE}${EVENT}\;")
endforeach()

# define slots
execute_process(COMMAND seq 200 202 OUTPUT_VARIABLE SLOTS)
string(REGEX REPLACE "[ \r\n]+" ";" SLOTS ${SLOTS})
message(STATUS "SLOTS: ${SLOTS}")
set(SSS "")
foreach(SLOT ${SLOTS})
  string(CONCAT SSS "${SSS}${SLOT}\;")
endforeach()

# entry
add_custom_command(
  OUTPUT dispatch.c
  COMMAND
    cat ${CMAKE_CURRENT_SOURCE_DIR}/dispatch.c.in | #
    $<TARGET_FILE:tmplr> -i #
    "-DCCC=${CCC}" "-DEEE=${EEE}" "-DSSS=${SSS}" | #
    $<TARGET_FILE:tmplr> -i #
    "-DCCC=${CCC}" "-DEEE=${EEE}" "-DSSS=${SSS}" -P_tmpl2 | #
    $<TARGET_FILE:tmplr> -i #
    "-DCCC=${CCC}" "-DEEE=${EEE}" "-DSSS=${SSS}" -P_tmpl3 #
    > dispatch.c
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dispatch.c.in)
add_custom_target(dispatch.c DEPENDS dispatch.c)

set(SRC dispatch.c)
set(TARGET dice-fastch)
add_library(${TARGET}.o OBJECT ${SRC})
add_dependencies(${TARGET}.o ${SRC})
target_link_libraries(${TARGET}.o PRIVATE dice.h)
target_compile_options(${TARGET}.o PRIVATE -fPIC)
