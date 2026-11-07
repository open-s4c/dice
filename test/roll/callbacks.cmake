include("${CMAKE_CURRENT_LIST_DIR}/common.cmake")


set(root "${WORK}/callbacks")
set(out "${root}/generated")
file(MAKE_DIRECTORY "${root}/app")
file(WRITE "${root}/app/slots.dice" [=[
(module closed (number 10) (callbacks false) (subscribes (CHAIN_FLOW ANY_EVENT)))
(module open (number 20) (subscribes (CHAIN_FLOW EVENT_TEST ANY_EVENT)))
(module nested
  (slot closed (number 30) (subscribes (CHAIN_FLOW ANY_EVENT)))
  (slot reopened (number 40) (callbacks true) (subscribes (CHAIN_FLOW ANY_EVENT))))
(module linked (number 60) (subscribes (CHAIN_FLOW ANY_EVENT)))
]=])
file(WRITE "${root}/policy.dice" [=[
(module app
  (module nested (callbacks false))
  (callbacks true))
]=])
set(imports "(import (app slots))\n")
set(policy "(include \"policy.dice\")\n")
set(common [=[
(mempool-size 1Mi)
(events EVENT_OTHER)
(module publisher (number 1) (publishes (CHAIN_FLOW ANY_EVENT)))
(module top (number 50) (subscribes (CHAIN_FLOW ANY_EVENT)))
]=])

roll_compile_flags(flags "${out}")
include("${FIXTURES}/core.cmake")
set(lib "${out}/runtime${suffix}")
set(mod "${out}/plugin${suffix}")
set(app "${out}/app")

foreach(default true false)
  set(config "${imports}${policy}(callbacks ${default})\n${common}")
  file(WRITE "${root}/system.dice" "${config}")
  run("${DICE}" "-I${root}" "${root}/system.dice" --query)
  foreach(expected "slot app/closed 10 forced callbacks false"
                   "slot app/open 20 forced callbacks true"
                   "slot app/nested/closed 30 forced callbacks false"
                   "slot app/nested/reopened 40 forced callbacks true"
                   "slot top 50 forced callbacks ${default}")
    if(NOT output MATCHES "${expected}")
      message(FATAL_ERROR "Missing '${expected}': ${output}")
    endif()
  endforeach()
  set(resolved "${output}")
  # Includes, imports, and reopened namespace settings are order independent.
  file(WRITE "${root}/reordered.dice"
       "(callbacks ${default})\n${common}${policy}${imports}")
  run("${DICE}" "-I${root}" "${root}/reordered.dice" --query)
  if(NOT output STREQUAL resolved)
    message(FATAL_ERROR "Callback policy depends on declaration order")
  endif()
  run("${DICE}" "-I${root}" "${root}/system.dice" -o "${out}")
  file(READ "${out}/dice.c" generated)
  if(generated MATCHES "dice_callbacks_[0-9]+_(10|30)\\[")
    message(FATAL_ERROR "Disabled slot has callback storage")
  endif()
  foreach(checked 0 1)
    foreach(lto 0 1)
      set(build_flags ${flags} -DDICE_CHECK_ROUTES=${checked})
      if(lto)
        list(APPEND build_flags -O3 -flto)
      endif()
      build_core(core_objects "${out}/dice.c" ${build_flags})
      run("${CC}" ${build_flags} -c "${out}/dice.c" -o "${out}/dice.o")
      run("${CC}" ${build_flags} -c "${FIXTURES}/callbacks/handlers.c"
          -o "${out}/handlers.o")
      run("${CC}" ${build_flags} ${shared} "${out}/dice.o" ${core_objects}
          "${out}/handlers.o" ${link} -o "${lib}")
      run("${CC}" ${build_flags} ${shared} ${plugin_flags}
          "${FIXTURES}/callbacks/plugin.c" -o "${mod}")
      run("${CC}" ${build_flags} "${FIXTURES}/callbacks/main.c" "${lib}"
          ${link} -o "${app}")
      run("${CMAKE_COMMAND}" -E env "${preload}=${lib}:${mod}" "${app}")
      if(NOT default)
        if(checked)
          expect_failure("undeclared subscribes" "${CMAKE_COMMAND}" -E env
                "${preload}=${lib}:${mod}" "${app}" extension)
        else()
          # PS_INVALID is nonzero; no callbacks may be registered in the tail.
          expect_failure("" "${CMAKE_COMMAND}" -E env
                "${preload}=${lib}:${mod}" "${app}" extension)
        endif()
      endif()
      run("${CC}" ${build_flags} -DOMIT_CLOSED -c
          "${FIXTURES}/callbacks/handlers.c" -o "${out}/partial.o")
      expect_failure("dice_slot_" "${CC}" ${build_flags} "${out}/dice.o"
            ${core_objects} "${out}/partial.o" "${FIXTURES}/callbacks/main.c"
            ${link} -o "${out}/missing")
    endforeach()
  endforeach()
endforeach()

# Duplicates are ambiguous only within the same scope, including reopened modules.
foreach(config "(callbacks true) (callbacks false)"
               "(module a (callbacks true)) (module a (callbacks false))"
               "(module a (callbacks true) (callbacks false))")
  file(WRITE "${root}/bad.dice" "${config}\n")
  expect_failure("duplicate" "${DICE}" --check "${root}/bad.dice")
endforeach()
foreach(config "(callbacks yes)" "(module a (callbacks yes))"
               "(module a (callbacks yes))")
  file(WRITE "${root}/bad.dice" "${config}\n")
  expect_failure("true or false" "${DICE}" --check "${root}/bad.dice")
endforeach()

# Only callbacks is accepted, including within modules and slots.
foreach(config "(plugins true)" "(module a (plugins true))"
               "(module a (plugins true))")
  file(WRITE "${root}/bad.dice" "${config}\n")
  expect_failure("unknown (form|slot field) plugins" "${DICE}" --check "${root}/bad.dice")
endforeach()

# Resolved policy participates in ABI validation even with identical IDs/routes.
file(WRITE "${root}/abi.dice" "${imports}${policy}${common}(callbacks false)\n")
run("${DICE}" "-I${root}" --query "${root}/abi.dice")
set(before "${output}")
file(READ "${root}/app/slots.dice" slots)
string(REPLACE "(callbacks false)" "(callbacks true)" slots "${slots}")
file(WRITE "${root}/app/slots.dice" "${slots}")
run("${DICE}" "-I${root}" --query "${root}/abi.dice")
string(REGEX MATCH "configuration [^\n]*" before_id "${before}")
string(REGEX MATCH "configuration [^\n]*" after_id "${output}")
if(before_id STREQUAL after_id)
  message(FATAL_ERROR "Slot callback policy is missing from the ABI fingerprint")
endif()
