# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

if(${ENABLE_SANITIZER})
  # if("${DICE_SANITIZER}" STREQUAL "address")
  return()
endif()

add_executable(tls_destructor tls_destructor.cpp)
if(${ENABLE_SANITIZER})
  target_compile_options(tls_destructor PRIVATE ${LIBSAN_CXX_FLAGS})
  target_link_options(tls_destructor PRIVATE ${LIBSAN_LD_FLAGS})
  target_link_libraries(tls_destructor PRIVATE ${LIBSAN_LINK})
endif()
target_link_libraries(tls_destructor PRIVATE mock_checker pthread)
add_test(
  NAME tls_destructor
  COMMAND
    ${DICE_SCRIPT} #
    -pthread -malloc -self -preload $<TARGET_FILE:malloc_free_checker> #
    ${CMAKE_CURRENT_BINARY_DIR}/tls_destructor #
    COMMAND_EXPAND_LISTS)
