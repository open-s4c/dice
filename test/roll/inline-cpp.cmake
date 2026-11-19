include("${CMAKE_CURRENT_LIST_DIR}/common.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/core.cmake")
set(out "${WORK}/inline-cpp")
file(MAKE_DIRECTORY "${out}")
roll_compile_flags(flags "${out}" DISPATCH NO_CTOR)
foreach(callbacks false true)
  foreach(option "" "(option inline)")
    file(
      WRITE "${out}/system.dice"
      "(mempool-size 1Mi)
      (callbacks ${callbacks})
      (module source (number 1)
        (publishes (CHAIN_FLOW EVENT_TEST EVENT_OTHER)))
      (module fast (number 5) ${option}
        (subscribes (CHAIN_FLOW EVENT_TEST EVENT_OTHER)))")
    run("${DICE}" "${out}/system.dice" -o "${out}")
    foreach(checked 0 1)
      foreach(mode IN LISTS lto_modes)
        roll_optimization_flags(build_flags "${mode}" "${checked}")
        set(cpp_flags ${build_flags})
        list(REMOVE_ITEM cpp_flags -std=gnu11)
        # now.h has pre-existing C++ narrowing conversions; do not conflate
        # those diagnostics with handler linkage or suppress other warnings.
        list(APPEND cpp_flags -std=gnu++20 -Wno-narrowing)
        run("${CXX}" ${cpp_flags} -c "${FIXTURES}/inline/subscriber.cpp" -o
            "${out}/subscriber.o")
        run("${CC}" ${build_flags} -c "${out}/dice.c" -o "${out}/dice.o")
        build_core(core_objects "${out}/dice.c" ${build_flags})
        set(objects "${out}/dice.o" ${core_objects} "${out}/subscriber.o")
        set(lib "${out}/runtime${suffix}")
        run("${CXX}"
            ${build_flags}
            ${shared}
            ${objects}
            ${link}
            -o
            "${lib}")
        run("${CC}"
            ${build_flags}
            "${FIXTURES}/inline/cpp-main.c"
            "${lib}"
            ${link}
            -o
            "${out}/app")
        run("${out}/app")
        # C++ inline/COMDAT emission must never coalesce duplicate handlers.
        run("${CXX}" ${cpp_flags} -c "${FIXTURES}/inline/subscriber.cpp" -o
            "${out}/duplicate.o")
        expect_failure(
          "duplicate symbol|multiple definition"
          "${CXX}"
          ${build_flags}
          ${shared}
          ${objects}
          "${out}/duplicate.o"
          ${link}
          -o
          "${lib}")
        if(NOT callbacks)
          run("${CXX}"
              ${cpp_flags}
              -DPARTIAL
              -c
              "${FIXTURES}/inline/subscriber.cpp"
              -o
              "${out}/partial.o")
          expect_failure(
            "dice_slot_"
            "${CXX}"
            ${build_flags}
            ${shared}
            "${out}/dice.o"
            ${core_objects}
            "${out}/partial.o"
            ${link}
            -o
            "${lib}")
        endif()
      endforeach()
    endforeach()
  endforeach()
endforeach()
