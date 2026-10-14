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
file(MAKE_DIRECTORY "${root}/nested")
file(COPY "${FIXTURES}/cmake/" DESTINATION "${root}")
file(COPY "${SOURCE}/examples/roll/interceptors.dice" DESTINATION "${root}")
file(
  WRITE "${root}/foo.dice"
  [=[
(dice (schema 1) (runtime (plugins true))
  (include "interceptors.dice" "nested/observer-routes.dice"))
]=])
set(routes
    [=[
(dice (schema 1)
  (include "types-ids.dice")
  (slot observer (number 32) (after dice-self)
    (consumes (CAPTURE_BEFORE EVENT_MALLOC)
              (CAPTURE_EVENT EVENT_THREAD_START EVENT_THREAD_EXIT))))
]=])
file(WRITE "${root}/nested/observer-routes.dice" "${routes}")
file(WRITE "${root}/nested/types-ids.dice"
     "(dice (schema 1) (events (EVENT_USER 200)))\n")
file(
  WRITE "${root}/bar.dice"
  [=[
(dice (schema 1) (runtime (plugins false))
  (include "interceptors.dice")
  (events (EVENT_USER 400))
  (slot observer (number 64) (after dice-self)
    (consumes (CAPTURE_BEFORE EVENT_MALLOC)
              (CAPTURE_EVENT EVENT_THREAD_START EVENT_THREAD_EXIT))))
]=])
file(
  WRITE "${root}/direct.dice"
  [=[
(dice (schema 1) (runtime (plugins false))
  (slot source (produces (CHAIN_USER EVENT_USER)))
  (slot observer (consumes (CHAIN_USER EVENT_USER))))
]=])
set(configure
    "${CMAKE_COMMAND}"
    -S
    "${root}"
    -B
    "${build}"
    "-DDICE_SOURCE=${SOURCE}"
    "-DCMAKE_C_COMPILER=${CC}"
    "-DCMAKE_CXX_COMPILER=${CXX}")
if(GENERATOR)
  list(APPEND configure -G "${GENERATOR}")
endif()
if(MAKE)
  list(APPEND configure "-DCMAKE_MAKE_PROGRAM=${MAKE}")
endif()
run(${configure})
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

# A transitive include must regenerate foo.
file(WRITE "${root}/nested/types-ids.dice"
     "(dice (schema 1) (events (EVENT_USER 201)))\n")
run("${CMAKE_COMMAND}" --build "${build}" -j 4)
run("${build}/foo-app" 32 201)
file(TIMESTAMP "${build}/bar-dice/dice.h" now "%s")
if(NOT now STREQUAL bar_time)
  message(FATAL_ERROR "Changing foo regenerated unrelated bar configuration")
endif()

# Change the include graph itself, then edit the newly introduced dependency.
run("${CMAKE_COMMAND}" -E sleep 1)
string(REPLACE "types-ids.dice" "new-types.dice" routes "${routes}")
file(WRITE "${root}/nested/observer-routes.dice" "${routes}")
file(WRITE "${root}/nested/new-types.dice"
     "(dice (schema 1) (events (EVENT_USER 202)))\n")
run("${CMAKE_COMMAND}" --build "${build}" -j 4)
run("${build}/foo-app" 32 202)
run("${CMAKE_COMMAND}" -E sleep 1)
file(WRITE "${root}/nested/new-types.dice"
     "(dice (schema 1) (events (EVENT_USER 203)))\n")
string(REPLACE "(number 32)" "(number 33)" routes "${routes}")
file(WRITE "${root}/nested/observer-routes.dice" "${routes}")

# Moving a built-in slot must also rebuild the interceptor implementation.
file(READ "${root}/interceptors.dice" interceptors)
string(REPLACE "(slot dice-self" "(slot dice-self (number 7)" interceptors
               "${interceptors}")
file(WRITE "${root}/interceptors.dice" "${interceptors}")
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
