# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: 0BSD

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
target_link_libraries(
  tls_destructor PRIVATE malloc_free_check dice-malloc.o dice-pthread_create.o
                         dice-self.o dice.o)
add_test(NAME tls_destructor COMMAND tls_destructor)
