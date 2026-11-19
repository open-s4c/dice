# malloc_usable_size is not available on macOS.
if(NOT APPLE AND NOT ENABLE_SANITIZER)
  # NetBSD exposes the public malloc_usable_size wrapper through libjemalloc.
  set(usable_size_libraries)
  if(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
    list(APPEND usable_size_libraries jemalloc)
  endif()

  add_executable(malloc-usable-size-legacy usable_size.c)
  target_link_libraries(malloc-usable-size-legacy PRIVATE dice.h dice
                                                       ${usable_size_libraries})
  add_test(NAME malloc-usable-size-legacy
           COMMAND ${DICE_SCRIPT} -malloc
                   $<TARGET_FILE:malloc-usable-size-legacy>)

  if(NOT CMAKE_CROSSCOMPILING)
    add_executable(malloc-usable-size-roll rolled.c)
    add_dice_core(malloc-usable-size-roll usable_size.dice CHECK_ROUTES)
    add_dice_mods(malloc-usable-size-roll malloc)
    target_link_libraries(malloc-usable-size-roll PRIVATE ${usable_size_libraries})
    add_test(NAME malloc-usable-size-roll COMMAND malloc-usable-size-roll)
  endif()
endif()
