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
    "STATIC_ASSERT(CAPTURE_EVENT == 4, \"configured IDs\");\n")
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
  "STATIC_ASSERT(CAPTURE_EVENT == 4, \"ordinary IDs\");\n")
run("${CC}" ${flags} -c "${root}/ordinary.c" -o "${root}/ordinary.o")
