function(run)
  execute_process(COMMAND ${ARGV} RESULT_VARIABLE result OUTPUT_VARIABLE stdout
                  ERROR_VARIABLE stderr TIMEOUT 30)
  if(NOT result STREQUAL "0")
    message(FATAL_ERROR "${ARGV}: ${result}\n${stdout}${stderr}")
  endif()
endfunction()

set(root "${WORK}/header-contract")
file(MAKE_DIRECTORY "${root}")
file(WRITE "${root}/config.dice" "(schema 1)\n")
run("${DICE}" "${root}/config.dice" -o "${root}")
set(flags -std=gnu11 -D_GNU_SOURCE -Wall -Wextra -Werror
          "-I${root}" "-I${SOURCE}/include"
          "-I${SOURCE}/deps/libvsync/include")

# Defining the flag without supplying the configuration must fail at the
# first Dice header, including headers containing only identifier constants.
foreach(header compiler.h types.h pubsub.h module.h chains/capture.h
               chains/intercept.h events/dice.h events/cxa.h)
  file(WRITE "${root}/missing.c" "#include <dice/${header}>\n")
  execute_process(COMMAND "${CC}" ${flags} -DDICE_ROLLED=1 -c
                  "${root}/missing.c" -o "${root}/missing.o"
                  RESULT_VARIABLE result ERROR_VARIABLE stderr)
  if(result STREQUAL "0" OR NOT stderr MATCHES
      "DICE_ROLLED requires dice/roll.h or generated dice.h first")
    message(FATAL_ERROR "Missing header diagnostic for ${header}: ${stderr}")
  endif()
endforeach()

foreach(header dice.h dice/roll.h)
  file(WRITE "${root}/configured.c"
    "#include <${header}>\n#include <dice/module.h>\n"
    "#include <dice/chains/capture.h>\n"
    "STATIC_ASSERT(CHAIN_CAPTURE_EVENT == 4, \"configured IDs\");\n")
  run("${CC}" ${flags} -DDICE_ROLLED=1 -c "${root}/configured.c"
      -o "${root}/configured.o")
  # Including the configuration itself opts into generated mode too.
  run("${CC}" ${flags} -c "${root}/configured.c"
      -o "${root}/configured.o")
endforeach()

file(WRITE "${root}/forced.c" "#include <dice/module.h>\n")
run("${CC}" ${flags} -DDICE_ROLLED=1 -include dice/roll.h -c
    "${root}/forced.c" -o "${root}/forced.o")

# Ordinary sources continue using their existing includes and fixed IDs.
file(WRITE "${root}/ordinary.c"
  "#include <dice/module.h>\n#include <dice/chains/capture.h>\n"
  "#ifdef DICE_ROLLED\n#error ordinary source opted into generated mode\n#endif\n"
  "STATIC_ASSERT(CHAIN_CAPTURE_EVENT == 4, \"ordinary IDs\");\n")
run("${CC}" ${flags} -c "${root}/ordinary.c" -o "${root}/ordinary.o")

# Compare every ID (including aliases) against the single-header output.
file(WRITE "${root}/named.dice"
  "(schema 1) (events (EVENT_USER 201)) (chains (CHAIN_USER 21))\n")
run("${DICE}" "${root}/named.dice" -o "${root}" --name app)
file(READ "${root}/app.h" combined)
run("${DICE}" "${root}/named.dice" -o "${root}" --name app --ids-header app-ids.h)
file(READ "${root}/app.h" split)
file(READ "${root}/app-ids.h" ids)
execute_process(COMMAND "${DICE}" --query "${root}/named.dice"
                OUTPUT_VARIABLE query COMMAND_ERROR_IS_FATAL ANY)
string(REGEX MATCHALL "(event|chain) [A-Z_0-9]+ [0-9]+\n" entries "${query}")
foreach(entry IN LISTS entries)
  string(REGEX REPLACE "^(event|chain) " "#define " def "${entry}")
  string(FIND "${ids}" "${def}" standalone)
  string(FIND "${combined}" "${def}" original)
  string(FIND "${split}" "${def}" duplicate)
  if(standalone LESS 0 OR original LESS 0 OR NOT duplicate EQUAL -1)
    message(FATAL_ERROR "Missing or duplicated ID: ${def}")
  endif()
endforeach()

file(WRITE "${root}/guest.S" [=[
#include "app-ids.h"
#include "app-ids.h"
#if EVENT_USER != 201 || CHAIN_USER != 21 || CAPTURE_EVENT != 4
#error incorrect IDs
#endif
#ifdef DICE_ROLLED
#error standalone IDs header enabled the runtime
#endif
.long EVENT_USER, CHAIN_USER, CAPTURE_EVENT
]=])
run("${CC}" -nostdinc -c "${root}/guest.S" -o "${root}/guest.o")
file(WRITE "${root}/split.c"
  "#include \"app-ids.h\"\n#include \"app.h\"\n"
  "_Static_assert(EVENT_USER == 201 && CAPTURE_EVENT == 4, \"configured IDs\");\n")
run("${CC}" ${flags} -c "${root}/split.c" -o "${root}/split.o")

# Copying a different configuration's IDs header beside app.h must be diagnosed.
run("${DICE}" "${root}/config.dice" -o "${root}" --ids-header app-ids.h)
execute_process(COMMAND "${CC}" ${flags} -c "${root}/split.c" -o "${root}/split.o"
                RESULT_VARIABLE result ERROR_VARIABLE stderr)
if(result STREQUAL "0" OR NOT stderr MATCHES "IDs header does not match")
  message(FATAL_ERROR "Missing configuration mismatch diagnostic: ${stderr}")
endif()
