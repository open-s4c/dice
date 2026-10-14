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
