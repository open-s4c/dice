if(${ENABLE_SANITIZER})
  return()
endif()

file(GLOB SRCS *.c *.cpp)
foreach(SRC ${SRCS})
  get_filename_component(TARGET ${SRC} NAME_WLE)

  add_executable(${TARGET} ${SRC})
  target_compile_options(${TARGET} PRIVATE -fsanitize=thread)
  target_link_options(${TARGET} PRIVATE ${LIBSAN_SHARED})
  if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_link_options(${TARGET} PRIVATE -fsanitize=thread)
  endif()

  target_link_libraries(${TARGET} PRIVATE trace_checker dice-checkers.h dice
                                          dice.h)
  target_link_libraries(${TARGET} PRIVATE tsano pthread)
  add_test(
    NAME ${TARGET}-test
    COMMAND
      env DICE_BUILD=${PROJECT_BINARY_DIR} DICE_SOURCE=${PROJECT_SOURCE_DIR}
      ${PROJECT_SOURCE_DIR}/scripts/dice #
      -pthread -malloc -self -cxa -tsan #
      -preload $<TARGET_FILE:trace_checker> #
      ${CMAKE_CURRENT_BINARY_DIR}/${TARGET} #
      COMMAND_EXPAND_LISTS)
endforeach()
