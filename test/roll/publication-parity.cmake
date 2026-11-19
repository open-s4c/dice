include("${CMAKE_CURRENT_LIST_DIR}/common.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/core.cmake")
set(out "${WORK}/publication-parity")
file(MAKE_DIRECTORY "${out}")
roll_compile_flags(flags "${out}" DISPATCH NO_CTOR)
foreach(seal "" "(seal (dice pubsub))" "(seal _)")
  file(
    WRITE "${out}/system.dice"
    "(callbacks false) (mempool-size 1Mi)
    ${seal}
    (module app (number 1)
      (subscribes (CHAIN_CONTROL EVENT_DICE_INIT EVENT_DICE_READY)
                  (CHAIN_FLOW EVENT_TEST))
      (publishes (CHAIN_FLOW EVENT_TEST)))")
  run("${DICE}" "${out}/system.dice" -o "${out}")
  foreach(mode IN LISTS lto_modes)
    roll_optimization_flags(build_flags "${mode}" 0)
    build_core(core_objects "${out}/dice.c" ${build_flags})
    run("${CC}"
        ${build_flags}
        "${out}/dice.c"
        ${core_objects}
        "${FIXTURES}/publication-parity.c"
        ${link}
        -o
        "${out}/app")
    foreach(entry public internal header nested)
      # A separate process exercises the first publication for every path.
      run("${out}/app" "${entry}")
    endforeach()
  endforeach()
endforeach()
