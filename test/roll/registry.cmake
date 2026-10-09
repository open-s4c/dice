cmake_minimum_required(VERSION 3.20)

function(run)
  execute_process(COMMAND ${ARGV} RESULT_VARIABLE result OUTPUT_VARIABLE stdout
                  ERROR_VARIABLE stderr TIMEOUT 30)
  if(NOT result STREQUAL "0")
    message(FATAL_ERROR "${ARGV}: ${result}\n${stdout}${stderr}")
  endif()
  set(output "${stdout}${stderr}" PARENT_SCOPE)
endfunction()

function(fails)
  execute_process(COMMAND ${ARGV} RESULT_VARIABLE result OUTPUT_VARIABLE stdout
                  ERROR_VARIABLE stderr TIMEOUT 30)
  if(result STREQUAL "0" OR NOT "${stdout}${stderr}" MATCHES
                              "not declared by the generated configuration")
    message(FATAL_ERROR
            "Accepted an incorrect registration: ${ARGV}\n${stdout}${stderr}")
  endif()
endfunction()

set(root "${WORK}/registry")
set(out "${root}/generated")
file(MAKE_DIRECTORY "${out}")
set(flags -std=gnu11 -D_GNU_SOURCE -Wall -Wextra -Werror -fPIC
          "-I${out}" "-I${SOURCE}/include" "-I${SOURCE}/src/dice"
          "-I${SOURCE}/deps/libvsync/include")
include("${FIXTURES}/core.cmake")

foreach(callbacks true false)
  file(WRITE "${root}/system.dice" "(callbacks ${callbacks})
    (chains (CHAIN_CUSTOM 42)) (events (EVENT_CUSTOM 60000))\n")
  run("${DICE}" "${root}/system.dice" -o "${out}")
  run("${DICE}" --query "${root}/system.dice")
  # Every identifier advertised by the CLI must be available by name at runtime,
  # including built-in aliases that share an ID and sparse custom identifiers.
  string(REGEX MATCHALL "(chain|event) [A-Za-z_][A-Za-z0-9_]* [0-9]+"
                       identifiers "${output}")
  set(checks "")
  foreach(entry IN LISTS identifiers)
    string(REPLACE " " ";" fields "${entry}")
    list(GET fields 0 kind)
    list(GET fields 1 name)
    list(GET fields 2 id)
    if(kind STREQUAL "event")
      set(kind type)
    endif()
    string(APPEND checks "{
      _Static_assert(${name} == ${id}, \"identifier value\");
      const char *canonical = ps_${kind}_str(${name});
      assert(ps_${kind}_lookup(\"${name}\") == ${name});
      ps_register_${kind}(${name}, \"${name}\");
      assert(!strcmp(canonical, ps_${kind}_str(${name})));
    }\n")
  endforeach()
  file(WRITE "${root}/main.c" "#include \"dice.h\"
    #include <assert.h>
    #include <string.h>
    int main(int argc, char **argv) {
      if (argc > 1) {
        if (!strcmp(argv[1], \"event\"))
          ps_register_type(EVENT_CUSTOM, \"EVENT_THREAD_CREATE\");
        else
          ps_register_chain(CHAIN_CUSTOM, \"CHAIN_CONTROL\");
        return 0;
      }
      ${checks}
      assert(ps_type_lookup(NULL) == MAX_TYPES);
      assert(ps_type_lookup(\"missing\") == MAX_TYPES);
      assert(ps_chain_lookup(NULL) == MAX_CHAINS);
      assert(ps_chain_lookup(\"missing\") == MAX_CHAINS);
      return 0;
    }\n")
  foreach(lto 0 1)
    set(flags_saved ${flags})
    if(lto)
      list(APPEND flags -O2 -flto)
    endif()
    build_core(objects "${out}/dice.c")
    run("${CC}" ${flags} "${out}/dice.c" "${root}/main.c"
        ${objects} -pthread ${DL_LIBS} -o "${root}/app")
    run("${root}/app")
    fails("${root}/app" event)
    fails("${root}/app" chain)
    set(flags ${flags_saved})
  endforeach()
endforeach()
