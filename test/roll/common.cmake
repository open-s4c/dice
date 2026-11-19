include_guard(GLOBAL)

function(run)
  execute_process(
    COMMAND ${ARGV}
    RESULT_VARIABLE result
    OUTPUT_VARIABLE stdout
    ERROR_VARIABLE stderr
    TIMEOUT 30)
  if(NOT result STREQUAL "0")
    message(FATAL_ERROR "${ARGV}: ${result}\n${stdout}${stderr}")
  endif()
  set(output
      "${stdout}${stderr}"
      PARENT_SCOPE)
  set(result_text
      "${stdout}${stderr}"
      PARENT_SCOPE)
endfunction()

function(expect_failure expected)
  execute_process(
    COMMAND ${ARGN}
    RESULT_VARIABLE result
    OUTPUT_VARIABLE stdout
    ERROR_VARIABLE stderr
    TIMEOUT 30)
  if(result STREQUAL "0" OR NOT "${stdout}${stderr}" MATCHES "${expected}")
    message(
      FATAL_ERROR
        "Expected failure '${expected}': ${ARGN}\n${result}\n${stdout}${stderr}"
    )
  endif()
endfunction()

function(roll_compile_flags result directory)
  cmake_parse_arguments(FLAGS "DISPATCH;NO_CTOR;ROLLED" "" "" ${ARGN})
  set(options
      -std=gnu11
      -D_GNU_SOURCE
      -Wall
      -Wextra
      -Werror
      -fPIC
      "-I${directory}"
      "-I${SOURCE}/include"
      "-I${SOURCE}/src/dice"
      "-I${SOURCE}/deps/libvsync/include")
  if(FLAGS_DISPATCH)
    list(APPEND options -DDICE_DISPATCH_MODULE)
  endif()
  if(FLAGS_ROLLED)
    list(APPEND options -DDICE_ROLLED=1)
  endif()
  if(FLAGS_NO_CTOR)
    list(APPEND options -DDICE_DISABLE_PUBSUB_CTOR_INIT)
  endif()
  set(${result}
      "${options}"
      PARENT_SCOPE)
endfunction()

function(roll_optimization_flags result mode checked)
  set(options ${flags} -DDICE_CHECK_ROUTES=${checked})
  if(mode STREQUAL "thin")
    list(APPEND options -O3 -flto=thin)
  elseif(mode STREQUAL "full")
    list(APPEND options -O3 -flto)
  elseif(mode STREQUAL "debug-full")
    list(APPEND options -O2 -g -flto)
  elseif(mode STREQUAL "optimized")
    list(APPEND options -O2)
  endif()
  set(${result}
      "${options}"
      PARENT_SCOPE)
endfunction()

set(lto_modes none optimized full debug-full)
if(CC_ID MATCHES "Clang")
  list(APPEND lto_modes thin)
endif()
set(link -pthread ${DL_LIBS})
if(APPLE)
  set(shared -dynamiclib)
  set(suffix .dylib)
  set(plugin_flags -undefined dynamic_lookup)
  set(preload DYLD_INSERT_LIBRARIES)
else()
  set(shared -shared)
  set(suffix .so)
  set(plugin_flags)
  set(preload LD_PRELOAD)
endif()
