cmake_minimum_required(VERSION 3.20)
if(NOT BUILD)
  message(FATAL_ERROR "Pass -DBUILD=<configured Dice build directory>")
endif()
if(NOT DEFINED ITERATIONS)
  set(ITERATIONS 1000000)
endif()
if(NOT DEFINED SAMPLES)
  set(SAMPLES 5)
endif()
if(NOT ITERATIONS MATCHES "^[1-9][0-9]*$" OR NOT SAMPLES MATCHES
                                             "^[1-9][0-9]*$")
  message(FATAL_ERROR "ITERATIONS and SAMPLES must be positive integers")
endif()
include("${BUILD}/bench/roll/targets.cmake")
set(out "${BUILD}/bench/roll/results")
file(MAKE_DIRECTORY "${out}")
file(
  WRITE "${out}/samples.csv"
  "variant;publication;sample;iterations;elapsed_ns;ns_per_publish;checksum;library_bytes\n"
)
file(WRITE "${out}/sizes.txt" "")
find_program(size_tool NAMES llvm-size size REQUIRED)
find_program(dump_tool NAMES llvm-objdump objdump REQUIRED)
find_program(nm_tool NAMES llvm-nm nm REQUIRED)
foreach(target IN LISTS roll_benchmark_targets)
  string(REPLACE "|" ";" fields "${target}")
  list(GET fields 0 name)
  list(GET fields 1 executable)
  list(GET fields 2 library)
  file(SIZE "${library}" bytes)
  execute_process(
    COMMAND "${size_tool}" -A "${library}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE sizes
    ERROR_VARIABLE error)
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "${size_tool}: ${error}")
  endif()
  file(APPEND "${out}/sizes.txt" "${name}\n${sizes}\n")
  execute_process(
    COMMAND "${dump_tool}" -d "${library}"
    OUTPUT_FILE "${out}/${name}.asm"
    RESULT_VARIABLE result)
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "Could not disassemble ${library}")
  endif()
  # Retain the exported publication bodies separately for direct-call review.
  # Presence of an unused wrapper symbol alone does not imply a runtime call.
  file(STRINGS "${out}/${name}.asm" assembly)
  set(in_publication false)
  set(callpaths "")
  foreach(line IN LISTS assembly)
    if(line MATCHES "^[ \t]*[0-9a-fA-F]+ <_?roll_publish(_dynamic)?>:[ \t]*$")
      set(in_publication true)
    elseif(line MATCHES "^[ \t]*[0-9a-fA-F]+ <[^>]+>:[ \t]*$")
      set(in_publication false)
    endif()
    if(in_publication)
      string(APPEND callpaths "${line}\n")
    endif()
  endforeach()
  if(callpaths STREQUAL "")
    message(FATAL_ERROR "Publication bodies not found in ${name} disassembly")
  endif()
  file(WRITE "${out}/${name}.callpaths" "${callpaths}")
  execute_process(
    COMMAND "${nm_tool}" "${library}"
    OUTPUT_FILE "${out}/${name}.symbols"
    RESULT_VARIABLE result)
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "Could not inspect ${library} symbols")
  endif()
  foreach(publication constant dynamic)
    # This verifies real event delivery through Self, not just a fast no-op.
    execute_process(
      COMMAND "${executable}" 1000 "${publication}"
      RESULT_VARIABLE result
      OUTPUT_QUIET
      TIMEOUT 30)
    if(NOT result EQUAL 0)
      message(FATAL_ERROR "${name}/${publication}: smoke test failed")
    endif()
    foreach(sample RANGE 1 ${SAMPLES})
      execute_process(
        COMMAND "${executable}" "${ITERATIONS}" "${publication}"
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error
        TIMEOUT 60)
      if(NOT result EQUAL 0)
        message(FATAL_ERROR "${name}/${publication}: ${result}\n${error}")
      endif()
      string(STRIP "${output}" output)
      if(NOT output MATCHES "^[0-9]+;[0-9]+;[0-9.]+;[0-9]+$")
        message(FATAL_ERROR "Invalid benchmark output: ${output}")
      endif()
      file(APPEND "${out}/samples.csv"
           "${name};${publication};${sample};${output};${bytes}\n")
    endforeach()
  endforeach()
endforeach()
message(STATUS "Configured dispatch samples, sizes and call paths: ${out}")
