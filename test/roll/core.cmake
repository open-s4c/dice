# Compile the ordinary runtime sources against one generated configuration.
function(build_core result generated)
  get_filename_component(directory "${generated}" DIRECTORY)
  get_filename_component(name "${generated}" NAME_WE)
  set(objects)
  foreach(core pubsub mempool pubsub-box)
    set(object "${directory}/${core}.o")
    run("${CC}" ${flags} ${ARGN} -DDICE_ROLLED=1 -DDICE_ROLL_RUNTIME
        -DDICE_MODULE_SLOT=0 "-I${SOURCE}/include"
        "-I${SOURCE}/deps/libvsync/include" -include "${directory}/${name}.h"
        -c "${SOURCE}/src/dice/${core}.c" -o "${object}")
    list(APPEND objects "${object}")
  endforeach()
  set(${result} "${objects}" PARENT_SCOPE)
endfunction()
