# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

# These tests require preloading, which is tricky when using qemu to emulate
# other architectures. Therefore, we disable them if TOOLCHAIN_FILE is set.

if(CMAKE_TOOLCHAIN_FILE)
  return()
endif()

add_library(publisher.o OBJECT publisher.c)
target_link_libraries(publisher.o PRIVATE dice.h)

add_library(publisher SHARED)
target_link_libraries(publisher PRIVATE publisher.o dice)

add_library(subscriber1.o OBJECT subscriber.c)
target_link_libraries(subscriber1.o PRIVATE dice.h)
target_compile_options(subscriber1.o PRIVATE -DPOSITION=201)

add_library(subscriber1 SHARED)
target_link_libraries(subscriber1 PRIVATE subscriber1.o dice)

add_library(subscriber2.o OBJECT subscriber.c)
target_link_libraries(subscriber2.o PRIVATE dice.h)
target_compile_options(subscriber2.o PRIVATE -DPOSITION=202)

add_library(subscriber2 SHARED)
target_link_libraries(subscriber2 PRIVATE subscriber2.o dice)

add_library(empty_publisher SHARED empty_publisher.c)
target_link_libraries(empty_publisher PRIVATE dice.h)

add_executable(order_test order_test.c)
target_link_libraries(order_test PRIVATE dice.h empty_publisher)

set(DICESO ${CMAKE_CURRENT_BINARY_DIR}/../../src/dice/libdice.so)

add_test(
  NAME preload-1-then-2-test
  COMMAND
    env #
    LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}
    LD_PRELOAD=${DICESO}:libpublisher.so:libsubscriber1.so:libsubscriber2.so #
    ./order_test 2)

add_test(
  NAME preload-2-then-1-test
  COMMAND
    env #
    LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}
    LD_PRELOAD=${DICESO}:libpublisher.so:libsubscriber2.so:libsubscriber1.so #
    ./order_test 2)

add_library(subscriber12 SHARED)
target_link_libraries(subscriber12 PRIVATE publisher.o subscriber1.o
                                           subscriber2.o dice)

add_library(subscriber21 SHARED)
target_link_libraries(subscriber21 PRIVATE publisher.o subscriber2.o
                                           subscriber1.o dice)

add_test(NAME preload-12-test
         COMMAND env LD_LIBRARY_PATH=. LD_PRELOAD=${DICESO}:libsubscriber12.so
                 ./order_test 2)

add_test(NAME preload-21-test
         COMMAND env LD_LIBRARY_PATH=. LD_PRELOAD=${DICESO}:libsubscriber21.so
                 ./order_test 2)

add_library(subscriber21f SHARED)
target_link_libraries(subscriber21f PRIVATE publisher.o subscriber2.o
                                            subscriber1.o dice-fastch.o dice.o)

add_test(NAME preload-21f-test
         COMMAND env LD_LIBRARY_PATH=. LD_PRELOAD=libsubscriber21f.so
                 ./order_test 2)
