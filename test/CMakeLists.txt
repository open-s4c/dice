# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: 0BSD

# test-specific includes such as dice/ensure.h
include_directories(include)

# to ensure tsan and other compiler passes work, use builtin atomics
add_compile_options(-DVATOMIC_BUILTINS)

# checkers used for other tests
add_subdirectory(checkers)

# test configuration
set(TSAN_mp on)
set(TSAN_simple-tsan on)
set(DICE_simple on)
set(DICE_intercept_event on)

# Test cases in these lists will be compiled but not ran with sanitizers
set(IGNORE_address "pthread_exit_window")
set(IGNORE_thread "pthread_exit_window")
set(IGNORE_undefined)

# Compile test and add test case to ctest
file(GLOB SRCS *.c)
foreach(SRC ${SRCS})
  get_filename_component(TEST ${SRC} NAME_WLE)
  set(TARGET ${TEST})
  add_executable(${TARGET} ${SRC})
  target_link_libraries(${TARGET} PRIVATE dice.h)

  if(${TARGET} IN_LIST IGNORE_${DICE_SANITIZER})
    continue()
  endif()

  add_test(NAME ${TARGET} COMMAND ${TARGET})
endforeach()

# Additional configuration of some tests
target_link_libraries(simple PRIVATE dice-self.o dice.o)
target_link_libraries(intercept_event PRIVATE dice-self.o dice.o)

target_link_libraries(pthread_create_join PRIVATE dice-pthread_create.o dice.o)
target_link_libraries(pthread_create_exit PRIVATE dice-pthread_create.o dice.o)
target_link_libraries(pthread_create_detach PRIVATE dice-pthread_create.o
                                                    dice.o)
target_link_libraries(
  pthread_exit_window PRIVATE dice-pthread_create.o dice-malloc.o dice-self.o
                              dice.o)

# pthread must be last library to link with tests
foreach(SRC ${SRCS})
  get_filename_component(TARGET ${SRC} NAME_WLE)
  if(TSAN_${TARGET})
    target_compile_options(${TARGET} PRIVATE -fsanitize=thread)
    target_link_libraries(${TARGET} PRIVATE tsano)
    target_link_options(${TARGET} PRIVATE ${LIBSAN_SHARED})
  elseif(${ENABLE_SANITIZER})
    target_link_options(${TARGET} PRIVATE ${LIBSAN_LD_FLAGS})
    target_link_libraries(${TARGET} PRIVATE ${LIBSAN_LINK})
  endif()

  target_link_libraries(${TARGET} PRIVATE pthread)
endforeach()

add_custom_target(expand-tests)
add_subdirectory(self)
add_subdirectory(interpose)
add_subdirectory(tsan)
add_subdirectory(order)
add_subdirectory(log)
add_subdirectory(cxx)
add_subdirectory(switcher)
