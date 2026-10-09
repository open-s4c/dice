function(run)
  execute_process(COMMAND ${ARGV} RESULT_VARIABLE result OUTPUT_VARIABLE stdout
                  ERROR_VARIABLE stderr TIMEOUT 30)
  if(NOT result STREQUAL "0")
    message(FATAL_ERROR "${ARGV}: ${result}\n${stdout}${stderr}")
  endif()
  set(output "${stdout}${stderr}" PARENT_SCOPE)
endfunction()

function(fails expected)
  execute_process(COMMAND ${ARGN} RESULT_VARIABLE result OUTPUT_VARIABLE stdout
                  ERROR_VARIABLE stderr TIMEOUT 30)
  if(result STREQUAL "0" OR NOT "${stdout}${stderr}" MATCHES "${expected}")
    message(FATAL_ERROR "Expected '${expected}': ${ARGN}\n${stdout}${stderr}")
  endif()
endfunction()

function(contains pattern)
  if(NOT output MATCHES "${pattern}")
    message(FATAL_ERROR "Missing '${pattern}' in ${output}")
  endif()
endfunction()

set(root "${WORK}/imports")
set(first "${root}/first path")
set(second "${root}/second")
file(MAKE_DIRECTORY "${first}/app" "${first}/dice" "${second}/app")

# An optional top-level schema does not change the runtime or ABI.
set(body [=[(import (dice self) (dice malloc) (dice pthread_create))
(module observer (after (dice self))
  (subscribes (CAPTURE_EVENT EVENT_THREAD_START)))

]=])
file(WRITE "${root}/bare.dice" "${body}")
file(WRITE "${root}/versioned.dice" "(schema 1)\n${body}\n")
run("${DICE}" "${root}/bare.dice" -o "${root}/bare" --depfile "${root}/deps.d")
run("${DICE}" "${root}/versioned.dice" -o "${root}/versioned")
foreach(extension c h)
  file(SHA256 "${root}/bare/dice.${extension}" bare)
  file(SHA256 "${root}/versioned/dice.${extension}" versioned)
  if(NOT bare STREQUAL versioned)
    message(FATAL_ERROR "Schema changed generated dice.${extension}")
  endif()
endforeach()
file(READ "${root}/deps.d" output)
contains("/src/mod/self.dice")
contains("/src/mod/malloc.dice")
contains("/src/mod/pthread_create.dice")
foreach(empty "" "; empty config\n" "(schema 1)")
  file(WRITE "${root}/empty.dice" "${empty}")
  run("${DICE}" --check "${root}/empty.dice")
endforeach()

# Search paths are ordered and accept both -I forms in every CLI mode.
file(WRITE "${first}/app/types.dice" "(events (EVENT_IMPORTED 200))\n")
file(WRITE "${second}/app/types.dice" "(events (EVENT_IMPORTED 201))\n")
file(WRITE "${root}/search.dice" "(import (app types))\n")
run("${DICE}" -I "${first}" --query "${root}/search.dice" "-I${second}")
contains("event EVENT_IMPORTED 200")
run("${DICE}" --query -I "${second}" "${root}/search.dice" -I "${first}")
contains("event EVENT_IMPORTED 201")
run("${DICE}" --check "${root}/search.dice" -I "${first}")
run("${DICE}" --graph -I "${first}" "${root}/search.dice")
run("${DICE}" "${root}/search.dice" -I "${first}" -o "${root}/search")
file(WRITE "${first}/dice/self.dice" "(module replacement)\n")
foreach(invalid "(import)" "(import ())" "(import app)" "(import (app \"types\"))"
                "(import (../app types))" "(import (app/types))"
                "(import (app missing))")
  file(WRITE "${root}/invalid.dice" "${invalid}\n")
  fails("import|identifier|symbol" "${DICE}" --check "${root}/invalid.dice" -I "${first}")
endforeach()
file(WRITE "${first}/app/a.dice" "(import (app b))\n")
file(WRITE "${first}/app/b.dice" "(include \"a.dice\")\n")
file(WRITE "${root}/cycle.dice" "(import (app a))\n")
fails("cycle" "${DICE}" -I "${first}" --check "${root}/cycle.dice")
file(WRITE "${first}/app/b.dice" "(import (app a))\n")
fails("cycle" "${DICE}" -I "${first}" --check "${root}/cycle.dice")
foreach(args "-I" "--query;-I;${first}" "--query;--graph;${root}/bare.dice"
             "--check;${root}/bare.dice;-o;${root}/unused")
  fails("Usage:" "${DICE}" ${args})
endforeach()

# Every bundled module has a standalone definition beside its implementation.
file(GLOB modules "${SOURCE}/src/mod/*.dice")
foreach(module IN LISTS modules)
  get_filename_component(name "${module}" NAME_WE)
  file(WRITE "${root}/module.dice" "(import (dice ${name}))\n")
  run("${DICE}" --check "${root}/module.dice")
endforeach()

# Removed wrapper/settings syntax and unavailable follow-up forms fail clearly.
foreach(invalid
    "(dice)"
    "(dice (schema 1) (callbacks false))"
    "(runtime (callbacks false))"
    "(plugins false)"
    "(prioritize (chains CAPTURE_EVENT))"
    "(module a (option inline))"
    "(seal _)"
)
  file(WRITE "${root}/unsupported.dice" "${invalid}")
  fails("unknown form|unknown slot field" "${DICE}" --check "${root}/unsupported.dice")
endforeach()
