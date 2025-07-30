# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

add_executable(tls_destructor tls_destructor.cpp)
add_test(NAME tls_destructor
         COMMAND ${DICE_SCRIPT} -pthread -malloc -self -preload
                 $<TARGET_FILE:malloc_free_check> ./tls_destructor)
