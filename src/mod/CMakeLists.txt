# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

add_custom_command(
  OUTPUT tsan.c
  COMMAND env $<TARGET_FILE:tmplr> ${CMAKE_CURRENT_SOURCE_DIR}/tsan.c.in >
          tsan.c
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tsan.c.in)
add_custom_target(tsan.c DEPENDS tsan.c)

file(GLOB SRCS *.c *.cpp)
list(APPEND SRCS tsan.c)
foreach(SRC ${SRCS})
  get_filename_component(MODULE ${SRC} NAME_WLE)
  set(TARGET mod-${MODULE})
  add_library(${TARGET} SHARED ${SRC})
  target_link_libraries(${TARGET} PRIVATE dice.h)
  set_target_properties(${TARGET} PROPERTIES PREFIX "")
  install(TARGETS ${TARGET} DESTINATION lib/dice)

  add_library(${TARGET}.o OBJECT ${SRC})
  target_link_libraries(${TARGET}.o PRIVATE dice.h)
endforeach()

target_sources(mod-autocept PRIVATE autocept_list.S)
