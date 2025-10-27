# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

add_library(dice-checkers.h INTERFACE)
target_include_directories(
  dice-checkers.h INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

set(SRCS trace_checker.c malloc_free_checker.c)
foreach(SRC ${SRCS})
  get_filename_component(TEST ${SRC} NAME_WLE)
  set(TARGET ${TEST})

  add_library(${TARGET} SHARED ${SRC})
  target_link_libraries(${TARGET} PUBLIC dice dice.h)

  if(${ENABLE_SANITIZER})
    target_compile_options(${TARGET} PRIVATE ${LIBSAN_C_FLAGS})
    target_link_options(${TARGET} PRIVATE ${LIBSAN_LD_FLAGS})
    target_link_libraries(${TARGET} PRIVATE ${LIBSAN_LINK})
  endif()
endforeach()

target_link_libraries(trace_checker PRIVATE dice-self)

add_library(mock_checker SHARED mock_checker.c)
target_link_libraries(mock_checker PUBLIC dice.h dice-checkers.h)
