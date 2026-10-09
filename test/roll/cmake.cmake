cmake_minimum_required(VERSION 3.20)

function(run)
  execute_process(
    COMMAND ${ARGV}
    RESULT_VARIABLE result
    OUTPUT_VARIABLE stdout
    ERROR_VARIABLE stderr
    TIMEOUT 120)
  if(NOT result STREQUAL "0")
    message(FATAL_ERROR "${ARGV}: ${result}\n${stdout}${stderr}")
  endif()
endfunction()

set(root "${WORK}/cmake-consumer")
set(build "${root}/build")
# Keep prior runs' generated dependency state out of this fixture. Incremental
# rebuilds are checked below within the same invocation.
file(REMOVE_RECURSE "${root}")
set(import_directory "import path")
set(extra_directory "extra definitions")
if(MAKE AND GENERATOR MATCHES "Makefiles")
  execute_process(COMMAND "${MAKE}" -V MAKE_VERSION
                  RESULT_VARIABLE bsd_make OUTPUT_QUIET ERROR_QUIET)
  if(bsd_make STREQUAL "0")
    # BSD make cannot parse space-containing prerequisites. Keep that coverage
    # for GNU Make/Ninja here and for the independent CLI import tests.
    set(import_directory imports)
    set(extra_directory extra)
  endif()
endif()
file(MAKE_DIRECTORY "${root}/${import_directory}/app"
                    "${root}/${import_directory}/dice")
file(REMOVE_RECURSE "${root}/${extra_directory}")
file(MAKE_DIRECTORY "${root}/${extra_directory}")
file(WRITE "${root}/${extra_directory}/first.dice" "(module first)\n")
file(COPY "${FIXTURES}/cmake/" DESTINATION "${root}")
file(COPY "${FIXTURES}/interceptors.dice" DESTINATION "${root}")
file(
  WRITE "${root}/foo.dice"
  [=[
(callbacks true)
(include "interceptors.dice")
(import (app observer-routes))
(import (extra))
]=])
set(routes
    [=[
(schema 1)
  (include "types-ids.dice")
  (module observer (number 32) (after (dice self))
    (subscribes (CAPTURE_BEFORE EVENT_MALLOC)
              (CAPTURE_EVENT EVENT_THREAD_START EVENT_THREAD_EXIT)))
]=])
file(WRITE "${root}/${import_directory}/app/observer-routes.dice" "${routes}")
file(WRITE "${root}/${import_directory}/app/types-ids.dice"
     "(schema 1) (events (EVENT_USER 200))\n")
file(
  WRITE "${root}/bar.dice"
  [=[
(schema 1) (callbacks false)
  (include "interceptors.dice")
  (events (EVENT_USER 400))
  (module app (slot observer (number 64) (after (dice self))
    (subscribes (CAPTURE_BEFORE EVENT_MALLOC)
              (CAPTURE_EVENT EVENT_THREAD_START EVENT_THREAD_EXIT))))
]=])
file(
  WRITE "${root}/direct.dice"
  [=[
(schema 1) (callbacks false)
  (module source (publishes (CHAIN_USER EVENT_USER)))
  (module observer (subscribes (CHAIN_USER EVENT_USER)))
]=])
set(configure
    "${CMAKE_COMMAND}"
    -S
    "${root}"
    -B
    "${build}"
    "-DDICE_SOURCE=${SOURCE}"
    "-DDICE_TEST_IMPORT_PATH=${import_directory}"
    "-DDICE_TEST_EXTRA_PATH=${extra_directory}"
    "-DCMAKE_C_COMPILER=${CC}"
    "-DCMAKE_CXX_COMPILER=${CXX}")
if(GENERATOR)
  list(APPEND configure -G "${GENERATOR}")
endif()
if(MAKE)
  list(APPEND configure "-DCMAKE_MAKE_PROGRAM=${MAKE}")
endif()
run(${configure})
execute_process(
  COMMAND ${configure} -DBAD_STATIC=ON
  RESULT_VARIABLE result
  OUTPUT_VARIABLE stdout
  ERROR_VARIABLE stderr)
if(result STREQUAL "0" OR NOT "${stdout}${stderr}" MATCHES
                          "does not support STATIC_LIBRARY")
  message(FATAL_ERROR
          "Missing static-runtime diagnostic: ${stdout}${stderr}")
endif()
run(${configure} -DBAD_STATIC=OFF)
run("${CMAKE_COMMAND}" --build "${build}" -j 4)
run("${build}/foo-app" 32 200)
run("${build}/bar-app" 64 400)
run("${build}/direct")
run("${build}/objects-app")

file(TIMESTAMP "${build}/foo-dice/dice.h" foo_time "%s")
file(TIMESTAMP "${build}/bar-dice/dice.h" bar_time "%s")
run("${CMAKE_COMMAND}" -E sleep 1)
run("${CMAKE_COMMAND}" --build "${build}" -j 4)
file(TIMESTAMP "${build}/foo-dice/dice.h" now "%s")
if(NOT now STREQUAL foo_time)
  message(FATAL_ERROR "Unchanged configuration was regenerated")
endif()

# An initially absent higher-precedence file must replace the bundled import
# during an ordinary incremental build, without reconfiguring the project.
run("${CMAKE_COMMAND}" -E sleep 1)
file(COPY "${SOURCE}/src/mod/self.dice"
     DESTINATION "${root}/${import_directory}/dice")
file(APPEND "${root}/${import_directory}/dice/self.dice"
     "\n(events (EVENT_LATE_OVERRIDE 250))\n")
run("${CMAKE_COMMAND}" --build "${build}" -j 4)
foreach(runtime foo bar)
  file(READ "${build}/${runtime}-dice/dice.h" header)
  if(NOT header MATCHES "#define EVENT_LATE_OVERRIDE 250")
    message(FATAL_ERROR "Adding a file override did not regenerate ${runtime}")
  endif()
endforeach()
run("${build}/foo-app" 32 200)
run("${build}/bar-app" 64 400)
file(TIMESTAMP "${build}/bar-dice/dice.h" bar_time "%s")

# Directory membership is a dependency, including additions and removals.
# Some make implementations compare timestamps at one-second granularity.
run("${CMAKE_COMMAND}" -E sleep 1)
file(WRITE "${root}/${extra_directory}/second.dice" "(module second)\n")
run("${CMAKE_COMMAND}" --build "${build}" -j 4)
file(READ "${build}/foo-dice/dice.h" header)
if(NOT header MATCHES "#define SLOT_EXTRA_SECOND ")
  message(FATAL_ERROR "Adding a namespace member did not regenerate foo")
endif()
run("${CMAKE_COMMAND}" -E sleep 1)
file(REMOVE "${root}/${extra_directory}/second.dice")
run("${CMAKE_COMMAND}" --build "${build}" -j 4)
file(READ "${build}/foo-dice/dice.h" header)
if(header MATCHES "#define SLOT_EXTRA_SECOND ")
  message(FATAL_ERROR "Removing a namespace member did not regenerate foo")
endif()

# An include inside an imported library must regenerate foo.
# Some make implementations compare timestamps at one-second granularity.
run("${CMAKE_COMMAND}" -E sleep 1)
file(WRITE "${root}/${import_directory}/app/types-ids.dice"
     "(schema 1) (events (EVENT_USER 201))\n")
run("${CMAKE_COMMAND}" --build "${build}" -j 4)
run("${build}/foo-app" 32 201)
file(TIMESTAMP "${build}/bar-dice/dice.h" now "%s")
if(NOT now STREQUAL bar_time)
  message(FATAL_ERROR "Changing foo regenerated unrelated bar configuration")
endif()

# Change the include graph itself, then edit the newly introduced dependency.
run("${CMAKE_COMMAND}" -E sleep 1)
string(REPLACE "types-ids.dice" "new-types.dice" routes "${routes}")
file(WRITE "${root}/${import_directory}/app/observer-routes.dice" "${routes}")
file(WRITE "${root}/${import_directory}/app/new-types.dice"
     "(schema 1) (events (EVENT_USER 202))\n")
run("${CMAKE_COMMAND}" --build "${build}" -j 4)
run("${build}/foo-app" 32 202)
run("${CMAKE_COMMAND}" -E sleep 1)
file(WRITE "${root}/${import_directory}/app/new-types.dice"
     "(schema 1) (events (EVENT_USER 203))\n")
string(REPLACE "(number 32)" "(number 33)" routes "${routes}")
file(WRITE "${root}/${import_directory}/app/observer-routes.dice" "${routes}")

# Moving a built-in slot must also rebuild the interceptor implementation.
file(READ "${root}/${import_directory}/dice/self.dice" self)
string(REPLACE "(module self" "(module self (number 7)" self "${self}")
file(WRITE "${root}/${import_directory}/dice/self.dice" "${self}")
run("${CMAKE_COMMAND}" --build "${build}" -j 4)
run("${build}/foo-app" 33 203)
run("${build}/bar-app" 64 400)
run("${build}/direct")

execute_process(
  COMMAND ${configure} -DBAD_MODULE=dice-missing
  RESULT_VARIABLE result
  OUTPUT_VARIABLE stdout
  ERROR_VARIABLE stderr)
if(result STREQUAL "0" OR NOT "${stdout}${stderr}" MATCHES
                          "unknown or unsupported module")
  message(
    FATAL_ERROR "Missing diagnostic for unknown module: ${stdout}${stderr}")
endif()
# Restore a reusable successful build directory.
run(${configure} -DBAD_MODULE=)
