add_library(sxp STATIC sxp.c)
dice_configure_tool(sxp)
target_include_directories(sxp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(sxp PUBLIC c_std_99)

add_executable(dice-cli roll.c slots.c)
dice_configure_tool(dice-cli)
target_include_directories(dice-cli PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(dice-cli PRIVATE
                           DICE_SOURCE_DIR="${PROJECT_SOURCE_DIR}")
target_link_libraries(dice-cli PRIVATE tmplr-lib sxp)
set_target_properties(dice-cli PROPERTIES OUTPUT_NAME dice-roll)

if(CMAKE_CROSSCOMPILING)
  find_program(DICE_HOST_CC NAMES cc clang gcc NO_CMAKE_FIND_ROOT_PATH REQUIRED)
  set(host_features -D_GNU_SOURCE)
  if(CMAKE_HOST_SYSTEM_NAME STREQUAL "NetBSD")
    set(host_features -D_XOPEN_SOURCE=700)
  endif()
  set(DICE_HOST_EXECUTABLE "" CACHE FILEPATH "Optional native dice-roll generator")
  add_executable(dice-cli-host IMPORTED GLOBAL)
  if(DICE_HOST_EXECUTABLE)
    set_target_properties(dice-cli-host PROPERTIES
                          IMPORTED_LOCATION "${DICE_HOST_EXECUTABLE}")
  else()
    set(host_cli "${CMAKE_CURRENT_BINARY_DIR}/dice-roll-host")
    set(tmplr_source "${PROJECT_SOURCE_DIR}/deps/tmplr")
    set(tmplr_binary "${PROJECT_BINARY_DIR}/deps/tmplr")
    file(GLOB_RECURSE host_headers CONFIGURE_DEPENDS
         "${PROJECT_SOURCE_DIR}/include/dice/*.h")
    add_custom_command(
      OUTPUT "${host_cli}"
      COMMAND ${DICE_HOST_CC} ${host_features} -std=c99 -O2 -DTMPLR_NO_MAIN
              "-DDICE_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\""
              "-I${CMAKE_CURRENT_SOURCE_DIR}"
              "-I${PROJECT_SOURCE_DIR}/include"
              "-I${PROJECT_SOURCE_DIR}/deps/libvsync/include"
              "-I${tmplr_source}/include" "-I${tmplr_binary}"
              "${CMAKE_CURRENT_SOURCE_DIR}/roll.c"
              "${CMAKE_CURRENT_SOURCE_DIR}/slots.c"
              "${CMAKE_CURRENT_SOURCE_DIR}/sxp.c" "${tmplr_source}/tmplr.c"
              -o "${host_cli}"
      DEPENDS roll.c slots.c slots.h sxp.c sxp.h standard.h templates/public.in
              ${host_headers}
              "${tmplr_source}/tmplr.c" "${tmplr_source}/include/tmplr.h"
              "${tmplr_binary}/version.h"
      VERBATIM)
    add_custom_target(dice-build-cli-host DEPENDS "${host_cli}")
    add_dependencies(dice-build-cli-host tmplr-version)
    set_target_properties(dice-cli-host PROPERTIES IMPORTED_LOCATION "${host_cli}")
    add_dependencies(dice-cli-host dice-build-cli-host)
  endif()
endif()
