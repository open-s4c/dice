# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

add_subdirectory(checkers)

add_compile_options(-DVATOMIC_BUILTINS)
set(TSAN_mp on)
set(TSAN_simple-tsan on)
set(DICE_simple on)
set(DICE_intercept_event on)

file(GLOB SRCS *.c)
foreach(SRC ${SRCS})
  get_filename_component(TEST ${SRC} NAME_WLE)
  set(TARGET ${TEST})
  add_executable(${TARGET} ${SRC})
  add_test(NAME ${TARGET} COMMAND ${TARGET})
  target_link_libraries(${TARGET} PRIVATE dice.h)
endforeach()

target_link_libraries(simple PRIVATE mod-self.o dice.o)
target_link_libraries(intercept_event PRIVATE mod-self.o dice.o)

target_link_libraries(pthread_create_join PRIVATE mod-pthread_create.o dice.o)
target_link_libraries(pthread_create_exit PRIVATE mod-pthread_create.o dice.o)
target_link_libraries(pthread_create_detach PRIVATE mod-pthread_create.o dice.o)

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

add_subdirectory(interpose)
add_subdirectory(tsan)
add_subdirectory(order)
