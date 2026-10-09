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
  set(output
      "${stdout}${stderr}"
      PARENT_SCOPE)
endfunction()

function(fails expected)
  execute_process(
    COMMAND ${ARGN}
    RESULT_VARIABLE result
    OUTPUT_VARIABLE stdout
    ERROR_VARIABLE stderr
    TIMEOUT 30)
  if(result STREQUAL "0" OR NOT "${stdout}${stderr}" MATCHES "${expected}")
    message(FATAL_ERROR "Expected '${expected}': ${ARGN}\n${stdout}${stderr}")
  endif()
endfunction()

function(contains pattern)
  if(NOT output MATCHES "${pattern}")
    message(FATAL_ERROR "Missing '${pattern}' in ${output}")
  endif()
endfunction()

set(root "${WORK}/namespaces")
file(REMOVE_RECURSE "${root}")
file(MAKE_DIRECTORY "${root}/src/dice" "${root}/src/mod")
file(WRITE "${root}/src/dice/foo.dice"
     "(module s1 (subscribes (CAPTURE_EVENT EVENT_MA_READ)))\n")
file(
  WRITE "${root}/src/mod/bar.dice"
  [=[
(module bar
  (slot s1 (after (dice s1))
    (subscribes (CAPTURE_EVENT EVENT_MA_WRITE))))
]=])
set(paths "-I${root}/src" "-Idice=${root}/src/mod")
file(
  WRITE "${root}/explicit.dice"
  [=[
(import (dice foo) (dice bar))
(module first (before (dice s1)) (before (dice bar s1)))
(module last (after (dice)))
]=])
run("${DICE}" ${paths} --graph "${root}/explicit.dice")
contains("\"slot:first\" -> \"slot:dice/s1\"")
contains("\"slot:first\" -> \"slot:dice/bar/s1\"")
contains("\"slot:dice/s1\" -> \"slot:dice/bar/s1\"")
contains("\"slot:dice/s1\" -> \"slot:last\"")
contains("\"slot:dice/bar/s1\" -> \"slot:last\"")
run("${DICE}" ${paths} "${root}/explicit.dice" -o "${root}/generated")
file(READ "${root}/generated/dice.h" output)
contains("#define SLOT_DICE_S1 [0-9]+")
contains("#define SLOT_DICE_BAR_S1 [0-9]+")

# Whole-directory imports merge roots, sort filenames, and load each file once.
# Use a separate namespace to avoid also importing Dice's bundled definitions.
file(MAKE_DIRECTORY "${root}/src/app" "${root}/extras/nested")
file(WRITE "${root}/src/app/z.dice"
     "(module z)\n\n")
file(WRITE "${root}/extras/a.dice"
     "(module a)\n\n")
file(WRITE "${root}/extras/z.dice" "(module should-be-shadowed)\n")
file(WRITE "${root}/extras/nested/child.dice" "(module child)\n")
file(WRITE "${root}/extras/ignored.dice.in" "(invalid)\n")
set(paths "-I${root}/src" -I "app=${root}/extras")
file(
  WRITE "${root}/all.dice"
  [=[
(import (app a) (app))

(import (app z) (app))
]=])
run("${DICE}" ${paths} --query "${root}/all.dice")
contains("slot app/a ")
contains("slot app/z ")
if(output MATCHES "should_be_shadowed|slot app/nested/child")
  message(FATAL_ERROR "Bulk import ignored precedence or recursed: ${output}")
endif()
file(WRITE "${root}/sorted.dice" "(import (app))\n")
run("${DICE}" ${paths} --query "${root}/sorted.dice")
run("${DICE}"
    ${paths}
    "${root}/all.dice"
    -o
    "${root}/all"
    --depfile
    "${root}/all.d")
file(READ "${root}/all.d" output)
contains("${root}/src/app ")
contains("${root}/extras ")
contains("${root}/extras/a.dice")
contains("${root}/src/app/z.dice")

# Mapping a nested directory preserves both its logical namespace components.
file(WRITE "${root}/nested.dice" "(import (app nested))\n")
run("${DICE}" ${paths} --query "${root}/nested.dice")
contains("slot app/nested/child ")
run("${DICE}" "-Iapp/nested=${root}/extras/nested" --query
    "${root}/nested.dice")
contains("slot app/nested/child ")

# Lexical lookup includes forward declarations and parents; list paths are
# absolute. Modules may be reopened and includes retain the enclosing scope.
file(WRITE "${root}/included.dice" "(slot included (after s1))\n")
file(
  WRITE "${root}/lexical.dice"
  [=[
(module s1)
(module app
  (module bar
    (slot first (before later) (after s1))
    (slot later (after (s1)))
    (include "included.dice"))
  (slot s1))
(module app (module bar (slot s1)))
(module before-all (before (app)))
(module after-bar (after (app bar)))
]=])
run("${DICE}" --graph "${root}/lexical.dice")
contains("\"slot:app/bar/s1\" -> \"slot:app/bar/first\"")
contains("\"slot:app/bar/first\" -> \"slot:app/bar/later\"")
contains("\"slot:s1\" -> \"slot:app/bar/later\"")
contains("\"slot:app/bar/s1\" -> \"slot:app/bar/included\"")
contains("\"slot:before_all\" -> \"slot:app/s1\"")
contains("\"slot:before_all\" -> \"slot:app/bar/later\"")
contains("\"slot:app/bar/included\" -> \"slot:after_bar\"")
file(
  WRITE "${root}/parents.dice"
  [=[
(module global)
(module app
  (slot parent)
  (module bar (slot child (after parent global))))
]=])
run("${DICE}" --graph "${root}/parents.dice")
contains("\"slot:app/parent\" -> \"slot:app/bar/child\"")
contains("\"slot:global\" -> \"slot:app/bar/child\"")

# Standalone constraints allow applications to order imported definitions,
# including entire namespaces, without redeclaring any slots.
file(
  WRITE "${root}/ordering.dice"
  [=[
(before (early) (late))
(after finish (late))
(module early
  (slot a)
  (slot b)
  (after b a))
(module late (slot a) (slot b))
(module finish)
]=])
run("${DICE}" --graph "${root}/ordering.dice")
foreach(left a b)
  foreach(right a b)
    contains("\"slot:early/${left}\" -> \"slot:late/${right}\"")
  endforeach()
endforeach()
contains("\"slot:early/a\" -> \"slot:early/b\"")
contains("\"slot:late/a\" -> \"slot:finish\"")
contains("\"slot:late/b\" -> \"slot:finish\"")
set(standalone "${output}")
file(
  WRITE "${root}/ordering.dice"
  [=[
(module early
  (slot a (before (late)))
  (slot b (before (late)) (after a)))
(module late (slot a) (slot b))
(module finish (after (late)))
]=])
run("${DICE}" --graph "${root}/ordering.dice")
if(NOT output STREQUAL standalone)
  message(FATAL_ERROR "Standalone constraints differ from slot constraints")
endif()
foreach(config "(before)" "(after a)" "(before a b c)")
  file(WRITE "${root}/bad.dice" "${config}\n")
  fails("expects two ordering references" "${DICE}" --check "${root}/bad.dice")
endforeach()
file(WRITE "${root}/bad.dice" "(module a) (module b) (before a b) (after a b)\n")
fails("cycle" "${DICE}" --check "${root}/bad.dice")

# One physical file can contribute to different namespaces; aliases within one
# namespace are still deduplicated.
file(CREATE_LINK "${root}/extras/a.dice" "${root}/extras/alias.dice" SYMBOLIC)
file(WRITE "${root}/aliases.dice" "(import (one a) (one alias) (two a))\n")
run("${DICE}" "-Ione=${root}/extras" "-Itwo=${root}/extras" --query
    "${root}/aliases.dice")
contains("slot one/a ")
contains("slot two/a ")

foreach(
  config
  "(module app (slot s1)) (module app (slot s1))"
  "(module app (slot s1) (module s1))" "(module app (module s1) (slot s1))")
  file(WRITE "${root}/bad.dice" "${config}\n")
  fails("duplicate slot|both a slot and a module" "${DICE}" --check
        "${root}/bad.dice")
endforeach()
file(WRITE "${root}/bad.dice" "(module app_s1) (module app (slot s1))\n")
fails("collide in generated macros" "${DICE}" --check "${root}/bad.dice")
file(WRITE "${root}/bad.dice"
     "(module a (slot s (after (b)))) (module b (slot s (after (a))))\n")
fails("cycle" "${DICE}" --check "${root}/bad.dice")
foreach(config "(module)" "(module (bad))" "(module s (before ()))")
  file(WRITE "${root}/bad.dice" "${config}\n")
  fails("symbol|empty ordering" "${DICE}" --check
        "${root}/bad.dice")
endforeach()
foreach(mapping "app=" "=somewhere" "../app=somewhere" "app//nested=somewhere"
                "app/=somewhere")
  fails("empty import|invalid" "${DICE}" "-I${mapping}" --check
        "${root}/all.dice")
endforeach()

# Directory imports cannot hide cycles behind import-once behavior.
file(MAKE_DIRECTORY "${root}/cycle")
file(WRITE "${root}/cycle/a.dice" "(import (cycle))\n")
file(WRITE "${root}/cycle.dice" "(import (cycle))\n")
fails("cycle" "${DICE}" "-Icycle=${root}/cycle" --check "${root}/cycle.dice")
file(MAKE_DIRECTORY "${root}/empty")
file(WRITE "${root}/empty.dice" "(import (empty)) (module s (before (empty)))\n")
run("${DICE}" "-Iempty=${root}/empty" --check "${root}/empty.dice")

# The built-in mapping supports importing all shipped module definitions.
file(WRITE "${root}/builtin.dice" "(import (dice))\n")
run("${DICE}" --query "${root}/builtin.dice")
contains("slot dice/self ")
contains("slot dice/pthread_spinlock ")
