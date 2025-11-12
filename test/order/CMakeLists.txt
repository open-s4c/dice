# These tests require preloading, which is tricky when using qemu to emulate
# other architectures. Therefore, we disable them if TOOLCHAIN_FILE is set.

if(CMAKE_TOOLCHAIN_FILE)
  return()
endif()

if(${ENABLE_SANITIZER})
  return()
endif()

add_library(subscriber1.o OBJECT subscriber.c)
target_link_libraries(subscriber1.o PRIVATE dice.h)
target_compile_options(subscriber1.o PRIVATE -DPOSITION=101 -fPIC)

add_library(subscriber1 SHARED)
target_link_libraries(subscriber1 PRIVATE subscriber1.o dice)

add_library(subscriber2.o OBJECT subscriber.c)
target_link_libraries(subscriber2.o PRIVATE dice.h)
target_compile_options(subscriber2.o PRIVATE -DPOSITION=202 -fPIC)

add_library(subscriber2 SHARED)
target_link_libraries(subscriber2 PRIVATE subscriber2.o dice)

add_library(mock_publisher SHARED mock_publisher.c)
target_link_libraries(mock_publisher PRIVATE dice.h)

add_library(publisher.o OBJECT publisher.c)
target_link_libraries(publisher.o PRIVATE dice.h)
target_link_libraries(publisher.o PUBLIC mock_publisher)
target_compile_options(publisher.o PRIVATE -fPIC)

add_library(publisher SHARED)
target_link_libraries(publisher PRIVATE publisher.o dice)

add_executable(order_test order_test.c)
target_link_libraries(order_test PRIVATE dice.h mock_publisher)

set(DICESO ${CMAKE_CURRENT_BINARY_DIR}/../../src/dice/libdice.${SO})

add_test(
  NAME preload-1-then-2-test
  COMMAND
    env #
    ${LIBRARY_PATH}=${CMAKE_CURRENT_BINARY_DIR} ${DICE_SCRIPT} -preload
    libpublisher.${SO}:libsubscriber1.${SO}:libsubscriber2.${SO} #
    ./order_test 2 202)

add_test(
  NAME preload-2-then-1-test
  COMMAND
    env #
    ${LIBRARY_PATH}=${CMAKE_CURRENT_BINARY_DIR}
    ${PRELOAD}=${DICESO}:libpublisher.${SO}:libsubscriber2.${SO}:libsubscriber1.${SO} #
    ./order_test 2 202)

add_library(subscriber12 SHARED publisher.c)
target_link_libraries(subscriber12 PRIVATE subscriber1.o subscriber2.o dice)
target_link_libraries(subscriber12 PRIVATE mock_publisher)

add_library(subscriber21 SHARED)
target_link_libraries(subscriber21 PRIVATE publisher.o subscriber2.o
                                           subscriber1.o dice)
target_link_libraries(subscriber21 PRIVATE mock_publisher)

add_test(NAME preload-12-test
         COMMAND env ${LIBRARY_PATH}=${CMAKE_CURRENT_BINARY_DIR}
                 ${PRELOAD}=${DICESO}:libsubscriber12.${SO} ./order_test 2 202)

add_test(NAME preload-21-test
         COMMAND env ${LIBRARY_PATH}=${CMAKE_CURRENT_BINARY_DIR}
                 ${PRELOAD}=${DICESO}:libsubscriber21.${SO} ./order_test 2 202)

add_library(subscriber21f SHARED)
target_link_libraries(subscriber21f PRIVATE publisher.o subscriber2.o
                                            subscriber1.o dice.o)
target_link_libraries(subscriber21f PRIVATE mock_publisher)

add_test(NAME preload-21f-test
         COMMAND env ${LIBRARY_PATH}=${CMAKE_CURRENT_BINARY_DIR}
                 ${PRELOAD}=libsubscriber21f.${SO} ./order_test 2 202)

add_library(subscriber3.o OBJECT subscriber.c)
target_link_libraries(subscriber3.o PRIVATE dice.h)
target_compile_options(subscriber3.o PRIVATE -DPOSITION=303 -fPIC)

add_library(subscriber3 SHARED)
target_link_libraries(subscriber3 PRIVATE subscriber3.o subscriber21f)
target_link_libraries(subscriber3 PRIVATE mock_publisher)

add_test(
  NAME preload-3-and-21f-test
  COMMAND
    env ${LIBRARY_PATH}=${CMAKE_CURRENT_BINARY_DIR}
    ${PRELOAD}=libsubscriber3.${SO}:libsubscriber21f.${SO} ./order_test 3 303)
