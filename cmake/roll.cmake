include_guard(GLOBAL)

# Source properties are shared within a CMake directory. Resolve the header from
# the compiling target, so two libraries can compile the same source with
# different configurations without affecting an unconfigured target.
function(_dice_source target path slot runtime)
  get_source_file_property(configured "${path}" TARGET_DIRECTORY
                           ${target} DICE_ROLL_CONFIGURED)
  if(NOT configured)
    set(rolled "$<BOOL:$<TARGET_PROPERTY:DICE_ROLL_HEADER>>")
    set_property(
      SOURCE "${path}" TARGET_DIRECTORY ${target}
      APPEND
      PROPERTY
        COMPILE_OPTIONS
        "$<${rolled}:-include;$<TARGET_PROPERTY:DICE_ROLL_HEADER>;${ARGN}>")
    set(definitions "DICE_MODULE_SLOT=${slot}")
    if(runtime)
      list(APPEND definitions DICE_ROLL_RUNTIME)
    endif()
    set_property(
      SOURCE "${path}" TARGET_DIRECTORY ${target}
      APPEND
      PROPERTY COMPILE_DEFINITIONS "$<${rolled}:${definitions}>")
    set_property(SOURCE "${path}" TARGET_DIRECTORY ${target}
                 PROPERTY DICE_ROLL_CONFIGURED TRUE)
  endif()
  target_sources(${target} PRIVATE "${path}")
endfunction()

function(add_dice_core target config)
  cmake_parse_arguments(PARSE_ARGV 2 CORE "CHECK_ROUTES" "" "")
  if(CORE_UNPARSED_ARGUMENTS OR NOT config)
    message(
      FATAL_ERROR "Usage: add_dice_core(target config.dice [CHECK_ROUTES])")
  endif()
  if(NOT TARGET "${target}")
    message(FATAL_ERROR "add_dice_core: target '${target}' does not exist")
  endif()
  get_target_property(kind "${target}" TYPE)
  get_target_property(imported "${target}" IMPORTED)
  get_target_property(alias "${target}" ALIASED_TARGET)
  if(imported
     OR alias
     OR NOT
        kind
        MATCHES
        "^(SHARED_LIBRARY|MODULE_LIBRARY|STATIC_LIBRARY|OBJECT_LIBRARY|EXECUTABLE)$"
  )
    message(FATAL_ERROR "add_dice_core requires a local compiled target")
  endif()
  get_target_property(previous "${target}" DICE_ROLL_HEADER)
  if(previous)
    message(
      FATAL_ERROR "add_dice_core: '${target}' already has a configuration")
  endif()

  get_filename_component(source "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.."
                         ABSOLUTE)
  get_filename_component(config "${config}" ABSOLUTE BASE_DIR
                         "${CMAKE_CURRENT_SOURCE_DIR}")
  set(generated "${CMAKE_CURRENT_BINARY_DIR}/${target}-dice")
  set(header "${generated}/dice.h")
  set(generator dice-cli)
  if(CMAKE_CROSSCOMPILING)
    set(generator dice-cli-host)
  endif()
  add_custom_command(
    OUTPUT "${generated}/dice.c" "${header}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${generated}"
    COMMAND $<TARGET_FILE:${generator}> "${config}" --output "${generated}"
            --depfile "${generated}/dice.d"
    DEPENDS ${generator} "${config}"
    DEPFILE "${generated}/dice.d"
    VERBATIM)
  add_custom_target(${target}-dice-generate DEPENDS "${generated}/dice.c"
                                                    "${header}")

  # Separate downstream object targets can share IDs and generation
  # dependencies.
  set(interface "${target}-dice.h")
  add_library(${interface} INTERFACE)
  add_dependencies(${interface} ${target}-dice-generate)
  target_sources(${interface} INTERFACE "${header}")
  target_include_directories(
    ${interface} INTERFACE "${generated}" "${source}/include"
                           "${source}/deps/libvsync/include")
  target_compile_features(${interface} INTERFACE c_std_11)
  target_compile_definitions(
    ${interface} INTERFACE DICE_ROLLED=1 DICE_DISPATCH_MODULE
                           "DICE_CHECK_ROUTES=$<BOOL:${CORE_CHECK_ROUTES}>")
  set_target_properties(${interface}
                        PROPERTIES INTERFACE_POSITION_INDEPENDENT_CODE ON)
  if(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
    target_compile_definitions(${interface} INTERFACE _XOPEN_SOURCE=700)
  else()
    target_compile_definitions(${interface} INTERFACE _GNU_SOURCE)
  endif()
  find_package(Threads REQUIRED)
  target_link_libraries(${interface} INTERFACE Threads::Threads)
  target_link_libraries(${target} PRIVATE ${interface} ${CMAKE_DL_LIBS})
  target_include_directories(${target} PRIVATE "${source}/src/dice")
  set_target_properties(${target} PROPERTIES DICE_ROLL_HEADER "${header}")
  target_sources(${target} PRIVATE "${generated}/dice.c")
  foreach(core pubsub mempool pubsub-box)
    _dice_source(${target} "${source}/src/dice/${core}.c" 0 TRUE)
  endforeach()
endfunction()

function(add_dice_mods target)
  if(NOT TARGET "${target}")
    message(FATAL_ERROR "add_dice_mods: target '${target}' does not exist")
  endif()
  get_target_property(header "${target}" DICE_ROLL_HEADER)
  if(NOT header)
    message(
      FATAL_ERROR
        "add_dice_mods: call add_dice_core(${target} config.dice) first")
  endif()
  get_target_property(selected "${target}" DICE_ROLL_MODULES)
  if(NOT selected)
    set(selected)
  endif()
  foreach(module IN LISTS ARGN)
    string(REPLACE "-" "_" module "${module}")
    string(REGEX REPLACE "^dice_" "" module "${module}")
    if(NOT module MATCHES "^[a-z0-9_]+$")
      message(FATAL_ERROR "add_dice_mods: invalid Dice module '${module}'")
    endif()
    if(module IN_LIST selected)
      message(FATAL_ERROR "add_dice_mods: duplicate module '${module}'")
    endif()
    list(APPEND selected "${module}")
    set(catalog "dice-${module}.o")
    if(NOT TARGET "${catalog}")
      message(
        FATAL_ERROR "add_dice_mods: unknown or unsupported module '${module}'")
    endif()
    get_target_property(sources "${catalog}" SOURCES)
    get_target_property(directory "${catalog}" SOURCE_DIR)
    get_target_property(options "${catalog}" DICE_MODULE_OPTIONS)
    if(NOT options)
      set(options)
    endif()
    string(TOUPPER "${module}" slot)
    foreach(path IN LISTS sources)
      get_filename_component(path "${path}" ABSOLUTE BASE_DIR "${directory}")
      _dice_source(${target} "${path}" "SLOT_DICE_${slot}" FALSE ${options})
    endforeach()
    if(module STREQUAL "tsan")
      add_dependencies(${target} expand-tsan.c)
    endif()
    if(APPLE AND module STREQUAL "cxa")
      target_link_libraries(${target} PRIVATE c++)
    endif()
  endforeach()
  set_target_properties(${target} PROPERTIES DICE_ROLL_MODULES "${selected}")
endfunction()
