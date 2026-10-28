cmake_minimum_required(VERSION 3.20)

function(run)
  execute_process(
    COMMAND ${ARGV}
    RESULT_VARIABLE result
    OUTPUT_VARIABLE stdout
    ERROR_VARIABLE stderr
    TIMEOUT 30)
  if(NOT result STREQUAL "0")
    message(FATAL_ERROR "${ARGV}: ${result}\n${stdout}${stderr}")
  endif()
endfunction()

set(root "${WORK}/route-keywords")
file(MAKE_DIRECTORY "${root}")
set(routes
    [=[
  (subscribes (CHAIN_CAPTURE_EVENT EVENT_MA_READ)
              (CHAIN_DICE_CONTROL EVENT_DICE_INIT))
  (publishes (CHAIN_INTERCEPT_EVENT EVENT_MA_READ))
]=])

foreach(scope module slot)
  if(scope STREQUAL "module")
    set(config "(callbacks false) (module observer ${routes})")
  else()
    set(config "(callbacks false) (module observer (slot worker ${routes}))")
  endif()
  file(WRITE "${root}/valid.dice" "${config}")
  run("${DICE}" --check "${root}/valid.dice")
  run("${DICE}" "${root}/valid.dice" -o "${root}/${scope}")

  # Chain-name aliases remain supported independently of route keywords.
  string(REPLACE "CHAIN_CAPTURE_" "CAPTURE_" aliases "${config}")
  string(REPLACE "CHAIN_INTERCEPT_" "INTERCEPT_" aliases "${aliases}")
  string(REPLACE "CHAIN_DICE_CONTROL" "CHAIN_CONTROL" aliases "${aliases}")
  file(WRITE "${root}/chain-aliases.dice" "${aliases}")
  run("${DICE}" "${root}/chain-aliases.dice" -o "${root}/${scope}-aliases")
  foreach(extension c h)
    file(SHA256 "${root}/${scope}/dice.${extension}" canonical)
    file(SHA256 "${root}/${scope}-aliases/dice.${extension}" alias)
    if(NOT canonical STREQUAL alias)
      message(FATAL_ERROR "Chain aliases changed generated ${extension}")
    endif()
  endforeach()

  # Reject removed keywords alone, alongside valid routes, and when empty.
  foreach(keyword consumes produces)
    foreach(
      fields
      "(${keyword} (CAPTURE_EVENT EVENT_MA_READ))"
      "${routes} (${keyword} (CAPTURE_EVENT EVENT_MA_WRITE))" "(${keyword})")
      if(scope STREQUAL "module")
        set(config "(module observer ${fields})")
      else()
        set(config "(module observer (slot worker ${fields}))")
      endif()
      file(WRITE "${root}/invalid.dice" "${config}")
      execute_process(
        COMMAND "${DICE}" --check "${root}/invalid.dice"
        RESULT_VARIABLE result
        ERROR_VARIABLE stderr
        TIMEOUT 30)
      if(result STREQUAL "0" OR NOT stderr MATCHES
                                "unknown (form|slot field) ${keyword}")
        message(
          FATAL_ERROR "Expected rejection of ${keyword}: ${config}\n${stderr}")
      endif()
    endforeach()
  endforeach()
endforeach()

# Different chain spellings cannot declare duplicate routes or redefine IDs.
foreach(
  invalid
  "(module a (subscribes (CAPTURE_EVENT EVENT_MA_READ) (CHAIN_CAPTURE_EVENT EVENT_MA_READ)))"
  "(chains (CAPTURE_EVENT 90))"
  "(chains (CHAIN_CAPTURE_EVENT 90))")
  file(WRITE "${root}/invalid.dice" "${invalid}")
  execute_process(
    COMMAND "${DICE}" --check "${root}/invalid.dice"
    RESULT_VARIABLE result
    ERROR_VARIABLE stderr
    TIMEOUT 30)
  if(result STREQUAL "0" OR NOT stderr MATCHES "duplicate")
    message(
      FATAL_ERROR "Accepted conflicting chain aliases: ${invalid}\n${stderr}")
  endif()
endforeach()
