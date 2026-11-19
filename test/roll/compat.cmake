function(run)
  execute_process(COMMAND ${ARGV} RESULT_VARIABLE result OUTPUT_VARIABLE stdout
                  ERROR_VARIABLE stderr TIMEOUT 120)
  if(NOT result STREQUAL "0")
    message(FATAL_ERROR "${ARGV}: ${result}\n${stdout}${stderr}")
  endif()
endfunction()
run("${CMAKE_COMMAND}" -S "${FIXTURES}/compat" -B "${WORK}/compat"
    "-DDICE_SOURCE=${SOURCE}" "-DCMAKE_C_COMPILER=${CC}"
    "-DCMAKE_CXX_COMPILER=${CXX}")
run("${CMAKE_COMMAND}" --build "${WORK}/compat" --target compat-app -j 4)
run("${CTEST}" --test-dir "${WORK}/compat" --output-on-failure)

# An ordinary cross build must not configure the optional Dice host generator.
# Use native compilers with an explicit system to exercise that configuration.
set(cross "${WORK}/compat-cross")
file(MAKE_DIRECTORY "${cross}")
file(WRITE "${cross}/toolchain.cmake"
     "set(CMAKE_SYSTEM_NAME \"${CMAKE_HOST_SYSTEM_NAME}\")\n"
     "set(CMAKE_C_COMPILER \"${CC}\")\n"
     "set(CMAKE_CXX_COMPILER \"${CXX}\")\n")
run("${CMAKE_COMMAND}" -S "${FIXTURES}/compat" -B "${cross}"
    "-DDICE_SOURCE=${SOURCE}" "-DTMPLR_HOST_CC=${CC}"
    "-DCMAKE_TOOLCHAIN_FILE=${cross}/toolchain.cmake")
run("${CMAKE_COMMAND}" --build "${cross}" --target compat-app -j 4)
run("${cross}/compat-app")
