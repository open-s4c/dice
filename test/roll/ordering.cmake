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

function(edge before after present)
  string(FIND "${output}" "\"slot:${before}\" -> \"slot:${after}\"" position)
  if((present AND position EQUAL -1) OR (NOT present AND NOT position EQUAL -1))
    message(FATAL_ERROR "Expected ${before} -> ${after}: ${present}\n${output}")
  endif()
endfunction()

set(root "${WORK}/ordering")
file(MAKE_DIRECTORY "${root}/app" "${root}/dice")
file(WRITE "${root}/dice/self.dice" "(module self)\n")
file(WRITE "${root}/app/slots.dice" [=[
(module pct)
(module guard (before (dice self)))
(module early
  (slot normal)
  (slot late (after (dice self)))
  (module nested
    (after _ (dice self))
    (slot normal)
    (slot early (before (dice self)))))
]=])
file(WRITE "${root}/policy.dice" [=[
(module app (module early (before _ (dice self))))
]=])
set(imports "(import (app slots) (dice self))\n")
set(policy "(after _ (dice self))\n(include \"policy.dice\")\n")
set(other "(module outside) (module app_extra (slot outside))\n")
set(paths "-Iapp=${root}/app" "-Idice=${root}/dice")
file(WRITE "${root}/system.dice" "${policy}${imports}${other}")
run("${DICE}" ${paths} --graph "${root}/system.dice")
foreach(slot app/pct app/early/late app/early/nested/normal outside app_extra/outside)
  edge(dice/self "${slot}" TRUE)
  edge("${slot}" dice/self FALSE)
endforeach()
foreach(slot app/guard app/early/normal app/early/nested/early)
  edge("${slot}" dice/self TRUE)
  edge(dice/self "${slot}" FALSE)
endforeach()
edge(dice/self dice/self FALSE)
set(graph "${output}")
file(WRITE "${root}/reordered.dice" "${other}${imports}${policy}")
run("${DICE}" ${paths} --graph "${root}/reordered.dice")
if(NOT output STREQUAL graph)
  message(FATAL_ERROR "Scoped ordering depends on import/declaration order")
endif()

# Optional endpoints do not import definitions from the search path.
file(WRITE "${root}/app/optional.dice" "(module optional)\n")
file(WRITE "${root}/optional.dice" [=[
(module a (after (dice self)) (after missing) (before (app optional)))
(after _ (dice self))
(before (missing namespace) a)
(after a (missing namespace))
]=])
run("${DICE}" ${paths} --query "${root}/optional.dice")
set(optional "${output}")
if(output MATCHES "slot (dice/self|app/optional|missing)")
  message(FATAL_ERROR "Ordering implicitly imports slots: ${output}")
endif()
file(WRITE "${root}/plain.dice" "(module a)\n")
run("${DICE}" ${paths} --query "${root}/plain.dice")
if(NOT output STREQUAL optional)
  message(FATAL_ERROR "Absent ordering endpoints affect the configuration")
endif()

# A target namespace is wholly excluded from its wildcard owner's set.
file(WRITE "${root}/namespace.dice" [=[
(after _ (anchors))
(module anchors (slot a) (slot b))
(module app (slot normal) (slot early (before (anchors))))
]=])
run("${DICE}" --graph "${root}/namespace.dice")
foreach(anchor a b)
  edge("anchors/${anchor}" app/normal TRUE)
  edge(app/early "anchors/${anchor}" TRUE)
  edge("anchors/${anchor}" app/early FALSE)
endforeach()
edge(anchors/a anchors/b FALSE)
edge(anchors/b anchors/a FALSE)

# Defaults resolve bare references lexically; named rules replace only their pair.
file(WRITE "${root}/lexical.dice" [=[
(module anchor)
(module app
  (after _ anchor)
  (slot anchor)
  (slot a)
  (module nested (slot b)))
(before (app a) (app anchor))
(after (app nested b) (app a))
(module apple (slot outside))
]=])
run("${DICE}" --graph "${root}/lexical.dice")
edge(app/a app/anchor TRUE)
edge(app/anchor app/a FALSE)
edge(app/anchor app/nested/b TRUE)
edge(app/a app/nested/b TRUE)
edge(anchor app/a FALSE)
edge(app/anchor apple/outside FALSE)

# Conflicting defaults may be replaced by a more specific rule in either order.
foreach(config [=[
(module anchor)
(before _ anchor) (after _ anchor)
(module app (after _ (anchor)) (slot a))
]=] [=[
(module app (after _ (anchor)) (slot a))
(after _ anchor) (before _ anchor)
(module anchor)
]=])
  file(WRITE "${root}/overridden.dice" "${config}")
  run("${DICE}" --graph "${root}/overridden.dice")
  edge(anchor app/a TRUE)
  edge(app/a anchor FALSE)
endforeach()

# Only surviving constraints participate in cycle and forced-number validation.
foreach(config "(module a) (module b) (before _ a) (after _ a)"
               "(module a (before b)) (module b (before a)) (after _ a)"
               "(module a) (module b) (before a b) (after a b)"
               "(module a) (module b) (module c) (after _ a) (after c b) (after a c)")
  file(WRITE "${root}/cycle.dice" "${config}\n")
  fails("cycle" "${DICE}" --check "${root}/cycle.dice")
endforeach()
file(WRITE "${root}/forced.dice"
     "(after _ anchor) (module anchor (number 2))\n"
     "(module first (number 1) (before anchor))\n")
run("${DICE}" --check "${root}/forced.dice")

# Requirements validate presence after all imports, independently of ordering.
file(WRITE "${root}/required.dice" [=[
(require (dice self) (app) (empty))
(module empty)
(module app
  (require pct (dice self))
  (slot consumer (require pct (dice self)) (after pct)))
(import (app slots) (dice self))
]=])
run("${DICE}" ${paths} --graph "${root}/required.dice")
edge(app/pct app/consumer TRUE)
edge(dice/self app/consumer FALSE)
foreach(config "(require (app optional))"
               "(module a (require (missing)))"
               "(module a (slot child (require missing)))")
  file(WRITE "${root}/required-missing.dice" "${config}\n")
  fails("required slot or namespace is absent" "${DICE}" ${paths} --check
        "${root}/required-missing.dice")
endforeach()

foreach(config "(require)" "(module a (require))" "(module a (slot child (require)))")
  file(WRITE "${root}/bad.dice" "${config}\n")
  fails("at least one reference" "${DICE}" --check "${root}/bad.dice")
endforeach()
foreach(config "(before _ _)" "(after a _)" "(module a (slot child (after _)))"
               "(module a (slot _))" "(module _)" "(require _)" "(require (_))")
  file(WRITE "${root}/bad.dice" "${config}\n")
  fails("reserved|standalone ordering default" "${DICE}" --check "${root}/bad.dice")
endforeach()
foreach(config "(require ())" "(module a (slot child (require ())))")
  file(WRITE "${root}/bad.dice" "${config}\n")
  fails("empty ordering reference" "${DICE}" --check "${root}/bad.dice")
endforeach()
