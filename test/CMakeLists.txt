# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

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
set(IGNORE_address pthread_exit_window ensure_self_fini ensure_self_fini_error
                   ensure_self_fini_detach)
set(IGNORE_thread pthread_exit_window)
set(IGNORE_undefined)

# Dice script options
set(DICE_OPTS_thread_sequence -pthread -self -preload $<TARGET_FILE:id_checker>)
set(DICE_OPTS_simple -self)
set(DICE_OPTS_intercept_event -self)
set(DICE_OPTS_pthread_create_join -pthread)
set(DICE_OPTS_pthread_create_exit -pthread)
set(DICE_OPTS_pthread_create_detach -pthread)
set(DICE_OPTS_pthread_exit_window -pthread -malloc -self)
set(DICE_OPTS_ensure_self_fini -pthread -malloc -self)
set(DICE_OPTS_ensure_self_fini_error -pthread -malloc -self)
set(DICE_OPTS_ensure_self_fini_detach -pthread -malloc -self)

set(WILL_FAIL_ensure_self_fini_error TRUE)

# tsano script path
set(TSANOSH env TSANO_LIBDIR=${CMAKE_CURRENT_BINARY_DIR}/../deps/tsano/
            ${CMAKE_CURRENT_SOURCE_DIR}/../deps/tsano/tsano)

# Compile test and add test case to ctest
file(GLOB SRCS *.c)
set(TARGETS)
foreach(SRC ${SRCS})
  get_filename_component(TEST ${SRC} NAME_WLE)
  set(TARGET ${TEST})
  set(TARGETS ${TARGETS} ${TARGET})

  # create executable
  add_executable(${TARGET} ${SRC})
  target_link_libraries(${TARGET} PRIVATE dice dice.h)

  # only add it as a test if not in ignore-list
  if(NOT ${TARGET} IN_LIST IGNORE_${DICE_SANITIZER})
    add_test(NAME ${TARGET} COMMAND ${DICE_SCRIPT} ${DICE_OPTS_${TEST}}
                                    ${CMAKE_CURRENT_BINARY_DIR}/${TARGET})
    target_compile_options(${TARGET} PRIVATE -Wno-incompatible-pointer-types)
    if(WILL_FAIL_${TARGET})
      set_property(TEST ${TARGET} PROPERTY WILL_FAIL TRUE)
    endif()
  endif()
endforeach()

# Additional configuration of some tests
target_link_libraries(pthread_exit_window PRIVATE dice-self)
target_link_libraries(ensure_self_fini PRIVATE dice-self)
target_link_libraries(ensure_self_fini_error PRIVATE dice-self)
target_link_libraries(ensure_self_fini_detach PRIVATE dice-self)

# handle TASN sanitizer
foreach(TARGET ${TARGETS})
  if(TSAN_${TARGET})
    target_compile_options(${TARGET} PRIVATE -fsanitize=thread)
    target_link_options(${TARGET} PRIVATE ${LIBSAN_SHARED})
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
      target_link_options(${TARGET} PRIVATE -fsanitize=thread)
    endif()
    target_link_libraries(${TARGET} PRIVATE tsano)
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
add_subdirectory(traces)
add_subdirectory(mempool)
