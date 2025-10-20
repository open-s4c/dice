# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

add_executable(mempool_reuse_test mempool_reuse_test.c)
target_link_libraries(mempool_reuse_test PRIVATE dice.h dice.o)
add_test(NAME mempool-reuse-test COMMAND mempool_reuse_test)
