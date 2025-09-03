# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: 0BSD

add_library(malloc_free_check SHARED malloc_free_check.c)
if(${ENABLE_SANITIZER})
  target_compile_options(malloc_free_check PRIVATE ${LIBSAN_C_FLAGS})
  target_link_options(malloc_free_check PRIVATE ${LIBSAN_LD_FLAGS})
  target_link_libraries(malloc_free_check PRIVATE dice ${LIBSAN_LINK})
else()
  target_link_libraries(malloc_free_check dice)
endif()
