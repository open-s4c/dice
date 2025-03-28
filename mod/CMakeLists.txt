add_custom_target(
    tsan.c
    COMMAND env tmplr ${CMAKE_CURRENT_SOURCE_DIR}/tsan.c.in
            > ${CMAKE_CURRENT_SOURCE_DIR}/tsan.c)

file(GLOB SRCS *.c *.cpp)
foreach(SRC ${SRCS})
	get_filename_component(MODULE ${SRC} NAME_WLE)
	set(TARGET ${MODULE})
	add_library(${TARGET} SHARED ${SRC})
	target_link_libraries(${TARGET} PRIVATE bingo)
	set_target_properties(${TARGET} PROPERTIES PREFIX "")
endforeach()

target_sources(autocept PRIVATE autocept_list.S)
