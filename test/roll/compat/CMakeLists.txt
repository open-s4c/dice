cmake_minimum_required(VERSION 3.20)
project(downstream LANGUAGES C CXX)
set(DICE_TESTS OFF CACHE BOOL "" FORCE)
set(DICE_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(DICE_LTO OFF CACHE BOOL "" FORCE)
set(DICE_MAX_TYPES 256 CACHE STRING "" FORCE)
set(DICE_MAX_CHAINS 12 CACHE STRING "" FORCE)
set(DICE_LAST_DISPATCH_SLOT 3 CACHE STRING "" FORCE)
set(DICE_DISPATCH_CHAINS "1,4,0,11" CACHE STRING "" FORCE)
set(DICE_LOG_LEVEL FATAL CACHE STRING "" FORCE)
add_subdirectory("${DICE_SOURCE}" dice)

foreach(target dice dice.h dice.o dice-core.o dice-box.o dice-mempool-box.o
               dice-pubsub-box.o dice-override.o dice-self.o dice-self-box.o
               dice-malloc.o dice-cxa.o dice-mman.o dice-pthread_create.o
               dice-pthread_mutex.o dice-pthread_cond.o dice-pthread_rwlock.o
               dice-sem.o dice-stacktrace.o dice-tsan.o tsano tmplr)
  if(NOT TARGET ${target})
    message(FATAL_ERROR "Missing downstream target: ${target}")
  endif()
endforeach()
get_directory_property(excluded DIRECTORY "${DICE_SOURCE}/src/mod" EXCLUDE_FROM_ALL)
if(excluded)
  message(FATAL_ERROR "Default build excludes legacy modules")
endif()

add_library(compat-runtime SHARED subscriber.c)
target_compile_definitions(compat-runtime PRIVATE DICE_MODULE_SLOT=2)
target_link_libraries(compat-runtime PRIVATE dice.h dice.o dice-self.o
                                           dice-pthread_create.o)
add_executable(compat-app main.c)
target_link_libraries(compat-app PRIVATE dice.h compat-runtime)
enable_testing()
add_test(NAME downstream COMMAND compat-app)
