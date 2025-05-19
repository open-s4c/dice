cmake_minimum_required(VERSION 3.20)
project(
  bingo
  LANGUAGES C CXX ASM
  VERSION 0.2)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
  -Werror
  -Wall
  -Wextra
  -Wno-unused-variable
  -Wno-missing-braces
  -Wno-switch
  -Wno-unused-parameter
  -Wno-unused-variable
  -Wno-missing-field-initializers
  -Wno-unused-command-line-argument
  $<$<COMPILE_LANGUAGE:C>:-Wno-override-init>)

if(CMAKE_SYSTEM_NAME MATCHES "NetBSD")
  add_compile_options(-D_XOPEN_SOURCE=700)
else()
  add_compile_options(-D_GNU_SOURCE)
endif()

find_package(libvsync 4.2 CONFIG REQUIRED)
link_libraries(libvsync::vsync)

include(GNUInstallDirs)
include(CheckSymbolExists)
include(cmake/export.cmake)

enable_testing()
add_subdirectory(deps/tmplr)
add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(mod)
add_subdirectory(tsano)
add_subdirectory(test)
add_subdirectory(scripts)
add_subdirectory(bench/micro)

install(TARGETS bingo bingo.h bingo-core EXPORT ${PROJECT_TARGETS})
