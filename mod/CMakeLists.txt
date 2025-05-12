# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

add_custom_target(
  tsan.c COMMAND env $<TARGET_FILE:tmplr> ${CMAKE_CURRENT_SOURCE_DIR}/tsan.c.in
                 > ${CMAKE_CURRENT_SOURCE_DIR}/tsan.c)

file(GLOB SRCS *.c *.cpp)
foreach(SRC ${SRCS})
  get_filename_component(MODULE ${SRC} NAME_WLE)
  set(TARGET ${MODULE})
  add_library(${TARGET} SHARED ${SRC})
  target_link_libraries(${TARGET} PRIVATE bingo.h)
  set_target_properties(${TARGET} PROPERTIES PREFIX "")
  install(TARGETS ${TARGET} DESTINATION lib/bingo)

  add_library(${TARGET}.o OBJECT ${SRC})
  target_link_libraries(${TARGET}.o PRIVATE bingo.h)
endforeach()

target_sources(autocept PRIVATE autocept_list.S)
