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
    message(FATAL_ERROR "Expected '${expected}': ${ARGN}\n${result}\n${stdout}${stderr}")
  endif()
endfunction()

set(out "${WORK}/sealed")
file(MAKE_DIRECTORY "${out}")
set(common "(mempool-size 1Mi)
  (import (dice self))
  (module app (after (dice self))
    (publishes (INTERCEPT_EVENT EVENT_TEST))
    (subscribes (CAPTURE_EVENT EVENT_TEST)))")
set(flags -std=gnu11 -D_GNU_SOURCE -DDICE_DISPATCH_MODULE
          -DDICE_DISABLE_PUBSUB_CTOR_INIT -Wall -Wextra -Werror -fPIC
          "-I${out}" "-I${SOURCE}/include" "-I${SOURCE}/src/dice"
          "-I${SOURCE}/deps/libvsync/include")
set(link -pthread ${DL_LIBS})
if(APPLE)
  set(shared -dynamiclib)
  set(suffix .dylib)
else()
  set(shared -shared)
  set(suffix .so)
endif()
set(open_hash "")
foreach(policy open all pool self pubsub)
  set(config "${common}")
  set(mask 0)
  if(policy STREQUAL "all")
    string(PREPEND config "(seal _)\n")
    set(mask 7)
  elseif(policy STREQUAL "pool")
    string(PREPEND config "(seal (dice mempool))\n")
    set(mask 2)
  elseif(policy STREQUAL "self")
    string(APPEND config "\n(seal (dice self))\n")
    set(mask 4)
  elseif(policy STREQUAL "pubsub")
    string(APPEND config "\n(seal (dice pubsub))\n")
    set(mask 1)
  endif()
  file(WRITE "${out}/system.dice" "${config}")
  run("${DICE}" "${out}/system.dice" -o "${out}")
  file(READ "${out}/dice.h" header)
  string(REGEX MATCH "DICE_CONFIG_HASH 0x[0-9a-f]+ULL" hash "${header}")
  if(policy STREQUAL "open")
    set(open_hash "${hash}")
  elseif(hash STREQUAL open_hash)
    message(FATAL_ERROR "Sealing did not change the ABI fingerprint: ${policy}")
  endif()
  foreach(component PUBSUB MEMPOOL SELF)
    if(component STREQUAL "PUBSUB")
      set(bit 1)
    elseif(component STREQUAL "MEMPOOL")
      set(bit 2)
    else()
      set(bit 4)
    endif()
    math(EXPR enabled "${mask} & ${bit}")
    if(enabled AND NOT header MATCHES "#define DICE_${component}_SEALED 1")
      message(FATAL_ERROR "Missing ${component} sealing flag: ${policy}")
    elseif(NOT enabled AND header MATCHES "#define DICE_${component}_SEALED")
      message(FATAL_ERROR "Unexpected ${component} sealing flag: ${policy}")
    endif()
  endforeach()
  run("${DICE}" --query "${out}/system.dice")
  if(policy STREQUAL "all" OR policy STREQUAL "self")
    if(NOT output MATCHES "slot dice/self [0-9]+ callbacks false sealed true")
      message(FATAL_ERROR "Missing sealed slot policy: ${output}")
    endif()
  endif()
  foreach(lto 0 1)
    set(build_flags ${flags})
    if(lto)
      list(APPEND build_flags -O3 -flto)
    endif()
    run("${CC}" ${build_flags} -c "${out}/dice.c" -o "${out}/dice.o")
    set(objects "${out}/dice.o")
    # No box implementation is needed, including for dispatch-only pubsub.
    foreach(core pubsub mempool)
      run("${CC}" ${build_flags} -DDICE_ROLL_RUNTIME -DDICE_MODULE_SLOT=0
          -include "${out}/dice.h" -c "${SOURCE}/src/dice/${core}.c"
          -o "${out}/${core}.o")
      list(APPEND objects "${out}/${core}.o")
    endforeach()
    run("${CC}" ${build_flags} -DDICE_MODULE_SLOT=SLOT_DICE_SELF
        -include "${out}/dice.h" -c "${SOURCE}/src/mod/self.c"
        -o "${out}/self.o")
    run("${CC}" ${build_flags} -c "${FIXTURES}/sealed/probe.c"
        -o "${out}/probe.o")
    list(APPEND objects "${out}/self.o" "${out}/probe.o")
    set(lib "${out}/runtime${suffix}")
    run("${CC}" ${build_flags} ${shared} ${objects} ${link} -o "${lib}")
    run("${CC}" -D_GNU_SOURCE "${FIXTURES}/sealed/main.c" "${lib}" ${link}
        -o "${out}/app")
    run("${out}/app" "${mask}")
    if(policy STREQUAL "all")
      # Sealed definitions are strong: a replacement must fail to link.
      foreach(component pubsub mempool self)
        if(component STREQUAL "pubsub")
          set(name ps_publish)
          set(body "#include <dice/pubsub.h>
            enum ps_err ps_publish(chain_id chain, type_id type,
                                   void *event, struct metadata *md) {
              (void)chain; (void)type; (void)event; (void)md;
              return PS_OK;
            }")
        elseif(component STREQUAL "mempool")
          set(name mempool_alloc)
          set(body "#include <dice/mempool.h>
            void *mempool_alloc(size_t size) { (void)size; return 0; }")
        else()
          set(name self_id)
          set(body "#include <dice/self.h>
            thread_id self_id(struct metadata *md) {
              (void)md; return NO_THREAD;
            }")
        endif()
        file(WRITE "${out}/override.c" "${body}\n")
        run("${CC}" ${build_flags} -c "${out}/override.c" -o "${out}/override.o")
        fails("${name}" "${CC}" ${build_flags} ${shared} ${objects}
              "${out}/override.o" ${link} -o "${lib}")
      endforeach()
      # Box sources compiled with the same configuration are harmless.
      set(boxes)
      foreach(component pubsub mempool self)
        if(component STREQUAL "self")
          set(directory mod)
        else()
          set(directory dice)
        endif()
        run("${CC}" ${build_flags} -DDICE_ROLL_RUNTIME -DDICE_MODULE_SLOT=0
            -include "${out}/dice.h" -c "${SOURCE}/src/${directory}/${component}-box.c"
            -o "${out}/${component}-box.o")
        list(APPEND boxes "${out}/${component}-box.o")
      endforeach()
      run("${CC}" ${build_flags} ${shared} ${objects} ${boxes} ${link} -o "${lib}")
      run("${out}/app" "${mask}")
    endif()
  endforeach()
endforeach()

foreach(config "(seal)" "(seal ())" "(seal self)" "(seal _ _)"
               "(seal (unknown component))" "(seal (dice self) (dice self))"
               "(seal (dice self)) (seal (dice self))"
               "(module a (seal _))" "(module a (seal _))"
               "(options sealed)" "(module a (options sealed))"
               "(module a (options sealed))")
  file(WRITE "${out}/invalid.dice" "${config}")
  fails("expects|duplicate|unknown|empty|top level" "${DICE}" --check "${out}/invalid.dice")
endforeach()
foreach(config "(seal _) (callbacks true)"
               "(callbacks true) (seal _)"
               "(seal _) (module a (callbacks true))"
               "(seal (a)) (module a (slot b (callbacks true)))"
               "(module a (slot b (callbacks true))) (seal (a))"
               "(module a (slot b (callbacks true))) (seal (a b))"
               "(callbacks true) (seal (dice pubsub))")
  file(WRITE "${out}/invalid.dice" "${config}")
  fails("callbacks true.*sealed" "${DICE}" --check "${out}/invalid.dice")
endforeach()
# Matching must respect namespace boundaries; unrelated callbacks remain valid.
file(WRITE "${out}/scopes.dice" "(seal (a)) (module a (slot x))
  (module ab (slot x (callbacks true)))")
run("${DICE}" --query "${out}/scopes.dice")
if(NOT output MATCHES "slot a/x [0-9]+ callbacks false sealed true" OR
   NOT output MATCHES "slot ab/x [0-9]+ callbacks true")
  message(FATAL_ERROR "Incorrect sealing inheritance: ${output}")
endif()
# The application can seal several components, independent of directive order.
file(WRITE "${out}/first.dice" "(seal (dice self) (dice mempool))\n${common}")
file(WRITE "${out}/second.dice" "${common}\n(seal (dice mempool))\n(seal (dice self))")
run("${DICE}" "${out}/first.dice" -o "${out}/first")
run("${DICE}" "${out}/second.dice" -o "${out}/second")
foreach(extension c h)
  file(SHA256 "${out}/first/dice.${extension}" first)
  file(SHA256 "${out}/second/dice.${extension}" second)
  if(NOT first STREQUAL second)
    message(FATAL_ERROR "Seal directive order changed generated ${extension}")
  endif()
endforeach()

# The same flags also compile against legacy headers, without a generated core.
foreach(source dice/pubsub dice/registry dice/mempool mod/self)
  string(REPLACE "/" "-" object "${source}")
  run("${CC}" -std=gnu11 -D_GNU_SOURCE -Wall -Wextra -Werror -fPIC
      -DDICE_PUBSUB_SEALED -DDICE_SELF_SEALED -DDICE_MEMPOOL_SEALED
      -DMEMPOOL_SIZE=1048576
      "-I${SOURCE}/include" "-I${SOURCE}/deps/libvsync/include"
      -c "${SOURCE}/src/${source}.c" -o "${out}/legacy-${object}.o")
endforeach()
