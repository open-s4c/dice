include("${CMAKE_CURRENT_LIST_DIR}/common.cmake")

set(out "${WORK}/inline")
file(MAKE_DIRECTORY "${out}" "${out}/plain")
roll_compile_flags(flags "${out}" DISPATCH NO_CTOR)
list(APPEND flags "-I${FIXTURES}/inline")
include("${FIXTURES}/core.cmake")
foreach(callbacks false true)
  set(config "(callbacks ${callbacks})
    (module source (number 1) (publishes (CHAIN_FLOW EVENT_TEST)))
    (module app
      (module fast (number 5) (option inline)
        (subscribes (CHAIN_FLOW ANY_EVENT))
        (publishes (CHAIN_DONE EVENT_DONE) (CHAIN_FLOW EVENT_TEST)))
      (slot normal (number 6)
        (subscribes (CHAIN_FLOW ANY_EVENT) (CHAIN_DONE EVENT_DONE))))")
  file(WRITE "${out}/system.dice" "${config}")
  run("${DICE}" "${out}/system.dice" -o "${out}")
  file(READ "${out}/dice.h" header)
  string(REGEX MATCH "DICE_CONFIG_HASH 0x[0-9a-f]+ULL" hash "${header}")
  string(REPLACE "(option inline)" "" plain "${config}")
  file(WRITE "${out}/plain.dice" "${plain}")
  run("${DICE}" "${out}/plain.dice" -o "${out}/plain")
  file(READ "${out}/plain/dice.h" plain_header)
  if(NOT plain_header MATCHES "${hash}")
    message(FATAL_ERROR "Inlining policy changed the runtime ABI fingerprint")
  endif()
  execute_process(COMMAND "${DICE}" --query "${out}/system.dice"
                  OUTPUT_VARIABLE query RESULT_VARIABLE result)
  if(NOT result EQUAL 0 OR NOT query MATCHES
     "slot app/fast 5 forced callbacks ${callbacks} option inline")
    message(FATAL_ERROR "Missing slot inline option in query: ${query}")
  endif()
  foreach(checked 0 1)
    foreach(lto IN LISTS lto_modes)
      roll_optimization_flags(build_flags "${lto}" "${checked}")
      run("${CC}" ${build_flags} -c "${out}/dice.c" -o "${out}/dice.o")
      build_core(core_objects "${out}/dice.c" ${build_flags})
      foreach(name handlers publisher)
        run("${CC}" ${build_flags} -c "${FIXTURES}/inline/${name}.c"
            -o "${out}/${name}.o")
      endforeach()
      set(objects "${out}/dice.o" ${core_objects} "${out}/handlers.o"
                  "${out}/publisher.o")
      set(lib "${out}/runtime${suffix}")
      run("${CC}" ${build_flags} ${shared} ${objects} ${link} -o "${lib}")
      run("${CC}" ${build_flags} "${FIXTURES}/inline/main.c" "${lib}"
          ${link} -o "${out}/app")
      run("${out}/app")
      if(NOT callbacks)
        # Inline wrappers must retain strong external linkage, so duplicate
        # consumers still fail to link rather than being silently coalesced.
        run("${CC}" ${build_flags} -DDUPLICATE -c
            "${FIXTURES}/inline/handlers.c" -o "${out}/duplicate.o")
        expect_failure("dice_slot_" "${CC}" ${build_flags} ${shared}
            ${objects} "${out}/duplicate.o" ${link} -o "${lib}")
      endif()
    endforeach()
  endforeach()
endforeach()
file(MAKE_DIRECTORY "${out}/self")
file(WRITE "${out}/self.dice" "(callbacks false) (import (dice self))")
run("${DICE}" "${out}/self.dice" -o "${out}/self")
# Catch-all self handlers must compile and link when republishing across
# chains, including the optimized GCC configurations that exposed the cycle.
foreach(lto IN LISTS lto_modes)
  roll_optimization_flags(self_flags "${lto}" 0)
  run("${CC}" ${self_flags} -DDICE_MODULE_SLOT=SLOT_DICE_SELF
      -include "${out}/self/dice.h" -c "${SOURCE}/src/mod/self.c"
      -o "${out}/self.o")
  build_core(self_objects "${out}/self/dice.c" ${self_flags})
  run("${CC}" ${self_flags} ${shared} "${out}/self/dice.c"
      ${self_objects} "${out}/self.o" ${link} -o "${out}/self/runtime${suffix}")
endforeach()
foreach(config "(module a (option))" "(module a (option inline extra))"
               "(module a (option inline) (option inline))"
               "(module a (option noinline))" "(option inline)")
  file(WRITE "${out}/bad.dice" "${config}")
  expect_failure("expects|duplicate|unknown|requires" "${DICE}" --check "${out}/bad.dice")
endforeach()
