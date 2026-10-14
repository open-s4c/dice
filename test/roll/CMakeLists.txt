if(NOT CMAKE_CROSSCOMPILING)
  set(roll_dl_libs ${CMAKE_DL_LIBS})
  list(TRANSFORM roll_dl_libs PREPEND "-l")
  if(NOT TARGET sxp-test)
    add_executable(sxp-test sxp.c)
    target_link_libraries(sxp-test PRIVATE sxp)
  endif()
  add_test(NAME roll-sxp COMMAND sxp-test)
  add_executable(roll-slots slots.c ${PROJECT_SOURCE_DIR}/src/cli/slots.c)
  target_include_directories(roll-slots PRIVATE ${PROJECT_SOURCE_DIR}/src/cli)
  add_test(NAME roll-slots COMMAND roll-slots)
  set_tests_properties(roll-slots PROPERTIES TIMEOUT 30)
  add_test(NAME roll COMMAND ${CMAKE_COMMAND}
           -DDICE=$<TARGET_FILE:dice-cli> -DCC=${CMAKE_C_COMPILER}
           -DWORK=${CMAKE_CURRENT_BINARY_DIR}
           -DFIXTURES=${CMAKE_CURRENT_SOURCE_DIR} -DAPPLE=${APPLE}
           "-DDL_LIBS=${roll_dl_libs}"
           -DSOURCE=${PROJECT_SOURCE_DIR}
           -P ${CMAKE_CURRENT_SOURCE_DIR}/integration.cmake)
  set_tests_properties(roll PROPERTIES TIMEOUT 120)
  add_test(NAME roll-legacy-cmake COMMAND ${CMAKE_COMMAND}
           -DCC=${CMAKE_C_COMPILER} -DCXX=${CMAKE_CXX_COMPILER}
           -DCTEST=${CMAKE_CTEST_COMMAND} -DSOURCE=${PROJECT_SOURCE_DIR}
           -DWORK=${CMAKE_CURRENT_BINARY_DIR}
           -DFIXTURES=${CMAKE_CURRENT_SOURCE_DIR}
           -P ${CMAKE_CURRENT_SOURCE_DIR}/compat.cmake)
  set_tests_properties(roll-legacy-cmake PROPERTIES TIMEOUT 180)
  add_test(NAME roll-dispatch-callback COMMAND ${CMAKE_COMMAND}
           -DDICE=$<TARGET_FILE:dice-cli> -DCC=${CMAKE_C_COMPILER}
           -DWORK=${CMAKE_CURRENT_BINARY_DIR}
           -DFIXTURES=${CMAKE_CURRENT_SOURCE_DIR} -DAPPLE=${APPLE}
           "-DDL_LIBS=${roll_dl_libs}"
           -DSOURCE=${PROJECT_SOURCE_DIR}
           -P ${CMAKE_CURRENT_SOURCE_DIR}/dispatch.cmake)
  set_tests_properties(roll-dispatch-callback PROPERTIES TIMEOUT 180)
  add_test(NAME roll-cmake COMMAND ${CMAKE_COMMAND}
           -DCC=${CMAKE_C_COMPILER} -DCXX=${CMAKE_CXX_COMPILER}
           -DSOURCE=${PROJECT_SOURCE_DIR} -DWORK=${CMAKE_CURRENT_BINARY_DIR}
           -DFIXTURES=${CMAKE_CURRENT_SOURCE_DIR}
           "-DGENERATOR=${CMAKE_GENERATOR}"
           "-DMAKE=${CMAKE_MAKE_PROGRAM}"
           -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake.cmake)
  set_tests_properties(roll-cmake PROPERTIES TIMEOUT 180)
  add_test(NAME roll-cross-cmake COMMAND ${CMAKE_COMMAND}
           -DCC=${CMAKE_C_COMPILER} -DCXX=${CMAKE_CXX_COMPILER}
           -DDICE=$<TARGET_FILE:dice-cli> -DSOURCE=${PROJECT_SOURCE_DIR}
           -DWORK=${CMAKE_CURRENT_BINARY_DIR}
           -DHOST_SYSTEM=${CMAKE_HOST_SYSTEM_NAME}
           "-DGENERATOR=${CMAKE_GENERATOR}"
           "-DMAKE=${CMAKE_MAKE_PROGRAM}"
           -P ${CMAKE_CURRENT_SOURCE_DIR}/cross.cmake)
  set_tests_properties(roll-cross-cmake PROPERTIES TIMEOUT 180)
  add_test(NAME roll-install COMMAND ${CMAKE_COMMAND}
           -DCC=${CMAKE_C_COMPILER} -DCXX=${CMAKE_CXX_COMPILER}
           -DSOURCE=${PROJECT_SOURCE_DIR} -DSO=${SO} -DPRELOAD=${PRELOAD}
           -DWORK=${CMAKE_CURRENT_BINARY_DIR}
           -P ${CMAKE_CURRENT_SOURCE_DIR}/install.cmake)
  set_tests_properties(roll-install PROPERTIES TIMEOUT 300)
endif()
