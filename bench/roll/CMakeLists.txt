set(DICE_ROLL_BENCH_LTO
    full
    CACHE STRING "Configured benchmark LTO mode")
set_property(CACHE DICE_ROLL_BENCH_LTO PROPERTY STRINGS none full thin)
if(NOT DICE_ROLL_BENCH_LTO MATCHES "^(none|full|thin)$")
  message(FATAL_ERROR "DICE_ROLL_BENCH_LTO must be none, full, or thin")
endif()
if(DICE_ROLL_BENCH_LTO STREQUAL "thin" AND NOT CMAKE_C_COMPILER_ID MATCHES
                                           "Clang")
  message(FATAL_ERROR "ThinLTO benchmarks require Clang")
endif()

set(targets)
set(manifest "set(roll_benchmark_targets\n")
foreach(policy default chains events)
  foreach(api open closed sealed)
    foreach(handler ordinary inline)
      set(name "roll-${policy}-${api}-${handler}")
      set(callbacks false)
      set(seal "")
      set(option "")
      set(priority "")
      if(api STREQUAL "open")
        set(callbacks true)
      elseif(api STREQUAL "sealed")
        set(seal "(seal _)")
      endif()
      if(handler STREQUAL "inline")
        set(option "(option inline)")
      endif()
      if(policy STREQUAL "chains")
        set(priority "(prioritize (chains INTERCEPT_EVENT CAPTURE_EVENT)
                                   (events EVENT_TEST))")
      elseif(policy STREQUAL "events")
        set(priority "(prioritize (events EVENT_TEST)
                                   (chains INTERCEPT_EVENT CAPTURE_EVENT))")
      endif()
      set(config "${CMAKE_CURRENT_BINARY_DIR}/${name}.dice")
      file(
        WRITE "${config}"
        "(mempool-size 1Mi) (callbacks ${callbacks})
        ${seal} ${priority}
        (import (dice self))
        (module source (after (dice self))
          (publishes (INTERCEPT_EVENT EVENT_TEST)))
        (module observer (after (dice self)) ${option}
          (subscribes (CAPTURE_EVENT EVENT_TEST)))")
      add_library(${name}-runtime SHARED EXCLUDE_FROM_ALL publisher.c
                                                          observer.c)
      add_dice_core(${name}-runtime "${config}")
      add_dice_mods(${name}-runtime self)
      target_compile_definitions(
        ${name}-runtime PRIVATE LOG_LEVEL=FATAL DICE_DISABLE_PUBSUB_CTOR_INIT)
      # Explicit flags make full and ThinLTO comparable instead of relying on
      # CMake's compiler-specific default IPO flavor.
      set_target_properties(${name}-runtime
                            PROPERTIES INTERPROCEDURAL_OPTIMIZATION OFF)
      if(NOT DICE_ROLL_BENCH_LTO STREQUAL "none")
        if(DICE_ROLL_BENCH_LTO STREQUAL "thin")
          set(lto -flto=thin)
        else()
          set(lto -flto)
        endif()
        target_compile_options(${name}-runtime PRIVATE ${lto})
        target_link_options(${name}-runtime PRIVATE ${lto})
      endif()
      add_executable(${name} EXCLUDE_FROM_ALL main.c)
      target_link_libraries(${name} PRIVATE ${name}-runtime)
      set_target_properties(${name} PROPERTIES INTERPROCEDURAL_OPTIMIZATION OFF)
      list(APPEND targets ${name})
      string(
        APPEND manifest
        "  \"${name}|$<TARGET_FILE:${name}>|$<TARGET_FILE:${name}-runtime>\"\n")
    endforeach()
  endforeach()
endforeach()
string(APPEND manifest ")\n")
file(
  GENERATE
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/targets.cmake"
  CONTENT "${manifest}")
file(
  GENERATE
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/results/build.txt"
  CONTENT
    "compiler=${CMAKE_C_COMPILER} ${CMAKE_C_COMPILER_VERSION}\nbuild_type=${CMAKE_BUILD_TYPE}\nlto=${DICE_ROLL_BENCH_LTO}\nc_flags=${CMAKE_C_FLAGS}\nrelease_flags=${CMAKE_C_FLAGS_RELEASE}\n"
)
add_custom_target(roll-benchmarks DEPENDS ${targets})
