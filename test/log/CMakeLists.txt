# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

add_executable(log-fatal-test log_levels.c)
target_link_libraries(log-fatal-test PRIVATE dice)
target_compile_options(log-fatal-test PRIVATE -DLOG_LEVEL=FATAL)
add_test(NAME log-fatal-test COMMAND log-fatal-test)

add_executable(log-info-test log_levels.c)
target_link_libraries(log-info-test PRIVATE dice)
target_compile_options(log-info-test PRIVATE -DLOG_LEVEL=INFO)
add_test(NAME log-info-test COMMAND log-info-test)

add_executable(log-debug-test log_levels.c)
target_link_libraries(log-debug-test PRIVATE dice)
target_compile_options(log-debug-test PRIVATE -DLOG_LEVEL=DEBUG)
add_test(NAME log-debug-test COMMAND log-debug-test)
