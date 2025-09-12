# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: 0BSD

# We link directly with dice-switcher.o and dice.o to avoid linking order issues
# when using the shared libraries.

add_executable(anythread_test anythread_test.c)
target_link_libraries(anythread_test dice-switcher.o dice.o pthread)
add_test(NAME anythread_test COMMAND anythread_test)

add_executable(yield_continue_test yield_continue_test.c)
target_link_libraries(yield_continue_test dice-switcher.o dice.o pthread)
add_test(NAME yield_continue_test COMMAND yield_continue_test)

add_executable(abort_test abort_test.c)
target_link_libraries(abort_test dice-switcher.o dice.o pthread)
add_test(NAME abort_test COMMAND abort_test)

add_executable(slack_test slack_test.c)
target_link_libraries(slack_test dice-switcher.o dice.o pthread)
add_test(NAME slack_test COMMAND slack_test)
