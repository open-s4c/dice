cmake_minimum_required(VERSION 3.20)
project(roll_consumer LANGUAGES C)
add_subdirectory("${DICE_SOURCE}" dice EXCLUDE_FROM_ALL)
find_package(Threads REQUIRED)

foreach(runtime foo bar)
  add_library(${runtime} SHARED)
  set(checks)
  if(runtime STREQUAL "bar")
    set(checks CHECK_ROUTES)
  endif()
  add_dice_core(${runtime} "${runtime}.dice" ${checks})
  add_dice_mods(${runtime} self pthread_create)
  add_dice_mods(${runtime} malloc)
  add_executable(${runtime}-app main.c)
  target_link_libraries(${runtime}-app PRIVATE ${runtime} Threads::Threads)
endforeach()
add_subdirectory(modules)

# Also support linking a complete runtime directly into an executable.
add_executable(direct direct.c)
add_dice_core(direct direct.dice CHECK_ROUTES)

# Core sources can also be added directly to an object target.
add_library(direct-objects OBJECT direct.c)
add_dice_core(direct-objects direct.dice CHECK_ROUTES)
add_executable(objects-app)
target_link_libraries(objects-app PRIVATE direct-objects)

# Source properties used by the helpers must not change an ordinary target
# compiling the same module in this directory.
add_library(ordinary-self OBJECT "${DICE_SOURCE}/src/mod/self.c")
target_link_libraries(ordinary-self PRIVATE dice.h)

# Exercise useful diagnostics without affecting the successful fixture build.
if(BAD_MODULE)
  add_library(bad SHARED observer.c)
  add_dice_core(bad foo.dice)
  add_dice_mods(bad "${BAD_MODULE}")
endif()
