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

# Ordinary cross builds only need the existing tmplr host tool. Prepare a
# native dice-roll executable when a consumer opts into generated dispatch.
function(_dice_add_host_cli)
  if(TARGET dice-cli-host)
    return()
  endif()
  get_target_property(cli_source dice-cli SOURCE_DIR)
  get_target_property(cli_binary dice-cli BINARY_DIR)
  get_target_property(tmplr_binary tmplr BINARY_DIR)
  get_filename_component(source "${cli_source}/../.." ABSOLUTE)
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
    set(host_cli "${cli_binary}/dice-roll-host")
    set(tmplr_source "${source}/deps/tmplr")
    set(tmplr_binary "${tmplr_binary}/tmplr")
    file(GLOB_RECURSE host_headers CONFIGURE_DEPENDS
         "${source}/include/dice/*.h")
    add_custom_command(
      OUTPUT "${host_cli}"
      COMMAND ${DICE_HOST_CC} ${host_features} -std=c99 -O2 -DTMPLR_NO_MAIN
              "-DDICE_SOURCE_DIR=\"${source}\""
              "-I${cli_source}"
              "-I${source}/include"
              "-I${source}/deps/libvsync/include"
              "-I${tmplr_source}/include" "-I${tmplr_binary}"
              "${cli_source}/roll.c"
              "${cli_source}/slots.c"
              "${cli_source}/sxp.c" "${tmplr_source}/tmplr.c"
              -o "${host_cli}"
      DEPENDS "${cli_source}/roll.c" "${cli_source}/slots.c"
              "${cli_source}/slots.h" "${cli_source}/sxp.c"
              "${cli_source}/sxp.h" "${cli_source}/standard.h"
              "${cli_source}/templates/public.in"
              ${host_headers}
              "${tmplr_source}/tmplr.c" "${tmplr_source}/include/tmplr.h"
              "${tmplr_binary}/version.h"
      VERBATIM)
    add_custom_target(dice-build-cli-host DEPENDS "${host_cli}")
    add_dependencies(dice-build-cli-host tmplr-version)
    set_target_properties(dice-cli-host PROPERTIES IMPORTED_LOCATION "${host_cli}")
    add_dependencies(dice-cli-host dice-build-cli-host)
  endif()
endfunction()
