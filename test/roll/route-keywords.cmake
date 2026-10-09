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
  (subscribes (CAPTURE_EVENT EVENT_MA_READ))
  (publishes (INTERCEPT_EVENT EVENT_MA_READ))
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
