cmake_minimum_required(VERSION 3.20)
project(rolled_example LANGUAGES C)

set(DICE_SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/../.."
    CACHE PATH "Dice source tree")
add_subdirectory("${DICE_SOURCE}" dice EXCLUDE_FROM_ALL)
option(CHECK_ROUTES "Validate declared routes at runtime" OFF)
set(checks)
if(CHECK_ROUTES)
  list(APPEND checks CHECK_ROUTES)
endif()

add_library(foo SHARED observer.c)
add_dice_core(foo "${CMAKE_CURRENT_SOURCE_DIR}/dice.dice" ${checks})
add_dice_mods(foo self pthread_create malloc)

find_package(Threads REQUIRED)
add_executable(sample main.c)
target_link_libraries(sample PRIVATE foo Threads::Threads)
enable_testing()
add_test(NAME capture-events COMMAND sample)
set_tests_properties(capture-events PROPERTIES PASS_REGULAR_EXPRESSION
                                               "linked observer: thread exit")
