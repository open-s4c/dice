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

set(root "${WORK}/modules")
file(REMOVE_RECURSE "${root}")
file(MAKE_DIRECTORY "${root}/src/app")
file(WRITE "${root}/system.dice" [=[
(callbacks false)
(module observer
  (number 8)
  (subscribes (CHAIN_FLOW EVENT_TEST))
  (slot allocations
    (number 12)
    (after default)
    (subscribes (CHAIN_FLOW EVENT_TEST))))
; Reopening adds fields to the same default slot.
(module observer (publishes (CHAIN_FLOW EVENT_TEST)))
(module group (slot child))
(module nested (module leaf))
(module empty)
]=])
run("${DICE}" --query "${root}/system.dice")
contains("slot observer 8 forced")
contains("slot observer/allocations 12 forced")
contains("slot group/child ")
contains("slot nested/leaf ")
contains("slot empty ")
if(output MATCHES "slot (group|nested) [0-9]")
  message(FATAL_ERROR "Container module allocated an unused default: ${output}")
endif()
run("${DICE}" --graph "${root}/system.dice")
contains("\"slot:observer\" -> \"slot:observer/allocations\"")
run("${DICE}" "${root}/system.dice" -o "${root}")
file(READ "${root}/dice.h" output)
contains("#define SLOT_OBSERVER 8")
contains("#define SLOT_OBSERVER_ALLOCATIONS 12")
if(output MATCHES "#define SLOT_(OBSERVER_DEFAULT|GROUP|NESTED) ")
  message(FATAL_ERROR "Unexpected generated slot constant: ${output}")
endif()

# Both slots in the same component must be callable and retain their order.
file(WRITE "${root}/main.c" [=[
#define DICE_MODULE_SLOT SLOT_OBSERVER
#include "dice.h"
#include <assert.h>
PS_SUBSCRIBE(CHAIN_FLOW, EVENT_TEST, {
    *(int *)event = *(int *)event * 10 + 1;
})
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, EVENT_TEST, SLOT_OBSERVER_ALLOCATIONS, {
    *(int *)event = *(int *)event * 10 + 2;
})
int main(void)
{
    int value = 0;
    assert(ps_publish(CHAIN_FLOW, EVENT_TEST, &value, NULL) == PS_OK);
    assert(value == 12);
    return 0;
}
]=])
set(flags -std=gnu11 -D_GNU_SOURCE -DDICE_DISABLE_PUBSUB_CTOR_INIT
          -DDICE_DISPATCH_MODULE -Wall -Wextra -Werror
          "-I${root}" "-I${SOURCE}/include" "-I${SOURCE}/src/dice"
          "-I${SOURCE}/deps/libvsync/include")
include("${FIXTURES}/core.cmake")
build_core(core_objects "${root}/dice.c")
run("${CC}" ${flags} "${root}/dice.c" "${root}/main.c" ${core_objects}
    -pthread ${DL_LIBS} -o "${root}/app")
run("${root}/app")

# A module reference includes the default and every descendant. The virtual
# default path selects only the implicit slot, including in required refs.
file(WRITE "${root}/groups.dice" [=[
(module observer
  (subscribes (CHAIN_FLOW EVENT_TEST))
  (slot child))
(module observer_extra)
(module first (before (observer)))
(module last (after (observer default)))
(require (observer default) (observer child))
]=])
run("${DICE}" --graph "${root}/groups.dice")
contains("\"slot:first\" -> \"slot:observer\"")
contains("\"slot:first\" -> \"slot:observer/child\"")
contains("\"slot:observer\" -> \"slot:last\"")
if(output MATCHES "\"slot:observer/child\" -> \"slot:last\"" OR
   output MATCHES "\"slot:first\" -> \"slot:observer_extra\"")
  message(FATAL_ERROR "Module/default reference selected extra slots: ${output}")
endif()

# Scoped defaults include the module's implicit slot, and exclude every slot
# belonging to the target module so that no self edges are introduced.
file(WRITE "${root}/defaults.dice" [=[
(module anchor (publishes (CHAIN_FLOW EVENT_TEST)) (slot child))
(after _ (anchor))
(module observer
  (subscribes (CHAIN_FLOW EVENT_TEST))
  (slot child)
  (module nested (subscribes (CHAIN_FLOW EVENT_TEST))))
]=])
run("${DICE}" --graph "${root}/defaults.dice")
foreach(left anchor anchor/child)
  foreach(right observer observer/child observer/nested)
    contains("\"slot:${left}\" -> \"slot:${right}\"")
  endforeach()
endforeach()

# Includes retain lexical module context. Imports start a new file at its
# logical namespace and cannot use an unwrapped slot declaration.
file(WRITE "${root}/parts.dice" "(slot child (after default))")
file(WRITE "${root}/include.dice"
     "(module app (publishes (CHAIN_FLOW EVENT_TEST)) (include \"parts.dice\"))")
run("${DICE}" --graph "${root}/include.dice")
contains("\"slot:app\" -> \"slot:app/child\"")
file(WRITE "${root}/src/app/parts.dice" "(slot invalid)")
file(WRITE "${root}/import.dice" "(import (app parts))")
fails("slot declarations must be inside a module" "${DICE}"
      "-I${root}/src" --check "${root}/import.dice")
# Import-once caching must not confuse an include's module scope with an
# import's top-level context, even when physical file and namespace match.
file(WRITE "${root}/cache.dice"
     "(module app (include \"src/app/parts.dice\")) (import (app parts))")
fails("slot declarations must be inside a module" "${DICE}"
      "-I${root}/src" --check "${root}/cache.dice")

foreach(config "(slot orphan)" "(module app) (slot orphan)"
               "(publishes (CHAIN_FLOW EVENT_TEST))"
               "(subscribes (CHAIN_FLOW EVENT_TEST))" "(number 4)")
  file(WRITE "${root}/bad.dice" "${config}")
  fails("inside a module|requires a module" "${DICE}" --check "${root}/bad.dice")
endforeach()
foreach(config "(module default)" "(module app (slot default))"
               "(module app (module default))")
  file(WRITE "${root}/bad.dice" "${config}")
  fails("default is reserved" "${DICE}" --check "${root}/bad.dice")
endforeach()
foreach(config
    "(module a (number 1)) (module a (number 2))"
    "(module a (publishes (CHAIN_FLOW EVENT_TEST))) (module a (publishes))"
    "(module a (subscribes (CHAIN_FLOW EVENT_TEST))) (module a (subscribes))")
  file(WRITE "${root}/bad.dice" "${config}")
  fails("duplicate slot field" "${DICE}" --check "${root}/bad.dice")
endforeach()
file(WRITE "${root}/bad.dice"
     "(module group (slot child)) (require (group default))")
fails("required slot or namespace is absent" "${DICE}" --check "${root}/bad.dice")
