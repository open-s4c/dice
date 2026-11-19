function(run)
  execute_process(COMMAND ${ARGV} RESULT_VARIABLE result OUTPUT_VARIABLE stdout
                  ERROR_VARIABLE stderr TIMEOUT 120)
  if(NOT result STREQUAL "0")
    message(FATAL_ERROR "${ARGV}: ${result}\n${stdout}${stderr}")
  endif()
endfunction()

# Setting the system explicitly exercises CMake's cross-compilation path using
# native compilers, so this test does not need a platform SDK or emulator.
set(root "${WORK}/cross-consumer")
set(build "${root}/build")
file(REMOVE_RECURSE "${root}")
file(MAKE_DIRECTORY "${root}")
file(COPY "${SOURCE}/test/roll/interceptors.dice" DESTINATION "${root}")
file(WRITE "${root}/closed.dice"
     "(callbacks false) (include \"interceptors.dice\")\n")
file(WRITE "${root}/CMakeLists.txt" [=[
cmake_minimum_required(VERSION 3.20)
project(cross_consumer LANGUAGES C CXX)
add_subdirectory("${DICE_SOURCE}" dice EXCLUDE_FROM_ALL)
foreach(target runtime closed)
  add_library(${target} SHARED)
  if(target STREQUAL "runtime")
    set(config "${CMAKE_CURRENT_SOURCE_DIR}/interceptors.dice")
  else()
    set(config "${CMAKE_CURRENT_SOURCE_DIR}/closed.dice")
  endif()
  add_dice_core(${target} "${config}")
  add_dice_mods(${target} self pthread_create malloc)
endforeach()
]=])
file(WRITE "${root}/toolchain.cmake"
     "set(CMAKE_SYSTEM_NAME \"${HOST_SYSTEM}\")\n"
     "set(CMAKE_C_COMPILER \"${CC}\")\n"
     "set(CMAKE_CXX_COMPILER \"${CXX}\")\n")
set(options "-DCMAKE_TOOLCHAIN_FILE=${root}/toolchain.cmake"
    "-DDICE_HOST_CC=${CC}" "-DTMPLR_HOST_CC=${CC}"
    "-DDICE_SOURCE=${SOURCE}"
    -DDICE_TESTS=OFF -DDICE_BENCHMARKS=OFF -DDICE_LTO=OFF)
if(GENERATOR)
  list(APPEND options -G "${GENERATOR}")
endif()
if(MAKE)
  list(APPEND options "-DCMAKE_MAKE_PROGRAM=${MAKE}")
endif()
run("${CMAKE_COMMAND}" -S "${root}" -B "${build}" ${options}
    -DDICE_HOST_EXECUTABLE=)
run("${CMAKE_COMMAND}" --build "${build}" --target runtime closed -j 4)
foreach(tool dice/src/cli/dice-roll dice/src/cli/libsxp.a dice/deps/libtmplr.a)
  if(EXISTS "${build}/${tool}")
    message(FATAL_ERROR "Cross consumer built an unused target tool: ${tool}")
  endif()
endforeach()

# Both generators must produce the same runtime, and an unchanged build must
# leave its generated files alone.
run("${DICE}" "${root}/interceptors.dice" -o "${root}/native")
foreach(extension c h)
  file(SHA256 "${root}/native/dice.${extension}" native_hash)
  file(SHA256 "${build}/runtime-dice/dice.${extension}" cross_hash)
  if(NOT native_hash STREQUAL cross_hash)
    message(FATAL_ERROR "Host generator produced different dice.${extension}")
  endif()
endforeach()
set(header "${build}/runtime-dice/dice.h")
file(TIMESTAMP "${header}" before "%s")
run("${CMAKE_COMMAND}" -E sleep 1)
run("${CMAKE_COMMAND}" --build "${build}" --target runtime -j 4)
file(TIMESTAMP "${header}" after "%s")
if(NOT before STREQUAL after)
  message(FATAL_ERROR "Unchanged cross build regenerated its runtime")
endif()

# An explicit host tool remains usable.
set(override "${root}/override")
run("${CMAKE_COMMAND}" -S "${root}" -B "${override}" ${options}
    "-DDICE_HOST_EXECUTABLE=${build}/dice/src/cli/dice-roll-host")
run("${CMAKE_COMMAND}" --build "${override}" --target runtime -j 4)
if(EXISTS "${override}/dice/src/cli/dice-roll-host")
  message(FATAL_ERROR "Cross build ignored the supplied host executable")
endif()
