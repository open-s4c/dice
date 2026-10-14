function(run)
  execute_process(COMMAND ${ARGV} RESULT_VARIABLE result OUTPUT_VARIABLE stdout
                  ERROR_VARIABLE stderr TIMEOUT 120)
  if(NOT result STREQUAL "0")
    message(FATAL_ERROR "${ARGV}: ${result}\n${stdout}${stderr}")
  endif()
endfunction()

set(build "${WORK}/install/build")
set(prefix "${WORK}/install/prefix")
file(REMOVE_RECURSE "${prefix}")
run("${CMAKE_COMMAND}" -S "${SOURCE}" -B "${build}"
    "-DCMAKE_C_COMPILER=${CC}" "-DCMAKE_CXX_COMPILER=${CXX}"
    "-DCMAKE_INSTALL_PREFIX=${prefix}"
    -DDICE_TESTS=OFF -DDICE_BENCHMARKS=OFF)
run("${CMAKE_COMMAND}" --build "${build}" -j 4)
run("${CMAKE_COMMAND}" --install "${build}")
if(EXISTS "${prefix}/bin/dice-roll" OR EXISTS "${prefix}/bin/dice-legacy")
  message(FATAL_ERROR "Source-tree generator must not change installed commands")
endif()
foreach(path include/dice/pubsub.h "lib/libdice.${SO}"
             "lib/dice/dice-malloc.${SO}" "lib/libtsano.${SO}"
             bin/dice bin/tsano lib/cmake/dice/diceConfig.cmake
             lib/cmake/dice/diceConfigVersion.cmake
             lib/cmake/dice/diceTargets.cmake)
  if(NOT EXISTS "${prefix}/${path}")
    message(FATAL_ERROR "Installation is missing ${path}")
  endif()
endforeach()
set(consumer "${WORK}/install/consumer")
file(MAKE_DIRECTORY "${consumer}")
file(WRITE "${consumer}/CMakeLists.txt" [=[
cmake_minimum_required(VERSION 3.20)
project(installed_consumer C)
find_package(dice 1.3 CONFIG REQUIRED)
if(NOT TARGET dice::dice OR NOT TARGET dice::dice.h)
  message(FATAL_ERROR "Missing exported Dice targets")
endif()
add_executable(consumer main.c)
target_link_libraries(consumer PRIVATE dice::dice)
]=])
file(WRITE "${consumer}/main.c" [=[
#include <dice/events/dice.h>
#include <dice/pubsub.h>
int main(void)
{
    return ps_publish(CHAIN_DICE_CONTROL, EVENT_DICE_NOP, 0, 0);
}
]=])
run("${CMAKE_COMMAND}" -S "${consumer}" -B "${consumer}/build"
    "-DCMAKE_C_COMPILER=${CC}"
    "-Ddice_DIR=${prefix}/lib/cmake/dice"
    "-Dlibvsync_DIR=${SOURCE}/deps/libvsync/share/libvsync/cmake")
run("${CMAKE_COMMAND}" --build "${consumer}/build")
run("${consumer}/build/consumer")
file(WRITE "${build}/sample.c" "#include <stdlib.h>\nint main(void) { void *p = malloc(1234); if (!p) return 1; free(p); return 0; }\n")
run("${CC}" "${build}/sample.c" -o "${build}/sample")
# Match the helper's core-first preload order: the core uses RTLD_NEXT to
# resolve allocator symbols. Loading it only as an interceptor dependency
# can place it after libc. Neither path may rely on an environment search
# path to locate installed dependencies under a custom prefix.
run("${CMAKE_COMMAND}" -E env --unset=LD_LIBRARY_PATH
    --unset=DYLD_LIBRARY_PATH --unset=LD_PRELOAD
    --unset=DYLD_INSERT_LIBRARIES
    "${PRELOAD}=${prefix}/lib/libdice.${SO}:${prefix}/lib/dice/dice-malloc.${SO}"
    "${build}/sample")
foreach(command "${prefix}/bin/dice" dice)
  foreach(options "-malloc" "")
    run("${CMAKE_COMMAND}" -E env --unset=LD_LIBRARY_PATH
        --unset=DYLD_LIBRARY_PATH --unset=LD_PRELOAD
        --unset=DYLD_INSERT_LIBRARIES "PATH=${prefix}/bin:$ENV{PATH}"
        DICE_BUILD= DICE_SOURCE= TSANO_LIBDIR=
        "${command}" ${options} "${build}/sample")
  endforeach()
endforeach()
