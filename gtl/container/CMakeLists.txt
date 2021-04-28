cmake_minimum_required(VERSION 3.6)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SRC
    pb_decoder.cpp
    st_decoder.cpp
    util.cpp
)

add_library(
    ${PROJECT_NAME} STATIC
    ${SRC}
)
add_dependencies(${PROJECT_NAME} LINK_HEADERS)

install(
    TARGETS ${PROJECT_NAME}
    ARCHIVE
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT devel
)

if (APPLE)
    set(LIBSO ${LIB_DIR}/libfastpb.a)
else ()
    set(LIBSO ${LIB_DIR}/libfastpb.so)
    add_custom_target(
        SCRIPT_SHARED_LIB ALL
        COMMAND ${CMAKE_COMMAND} -E echo 'GROUP ( libfastpb.a ) ' > ${LIBSO}
    )
    add_dependencies(SCRIPT_SHARED_LIB ${PROJECT_NAME})
endif ()

install(
    FILES ${LIBSO}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT devel
)