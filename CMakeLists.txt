cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -pipe -std=c++2a")
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type")
endif()
if(NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX /usr/local CACHE STRING "Install prefix")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")

project(gtl VERSION 0.0.1)
set(TEST_NAME ${PROJECT_NAME}_test)
enable_testing()

set(GTL_DIR ${PROJECT_SOURCE_DIR}/gtl)
set(INC_DIR ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME} CACHE PATH "gtl inc")
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib CACHE PATH "gtl lib")
set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin CACHE PATH "gtl bin")

include(GNUInstallDirs)

set(CMAKE_CONFIG_INSTALL_FILE ${PROJECT_BINARY_DIR}/config.toinstall.cmake)
set(CMAKE_CONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR})

set(INCLUDE_HEADERS
  gtl/algorithm/gtl_memory.h
  gtl/container/gtl_array.h
  gtl/container/gtl_deque.h
  gtl/container/gtl_list.h
  gtl/container/gtl_priority_queue.h
  gtl/container/gtl_queue.h
  gtl/container/gtl_slist.h
  gtl/container/gtl_stack.h
  gtl/container/gtl_storage.h
  gtl/container/gtl_vector.h
)
aux_source_directory(${GTL_DIR}/container/test CONTAINER_TEST_SRCS)
set(TEST_SRCS
  ${CONTAINER_TEST_SRCS}
)

add_custom_target(
  LINK_HEADERS ALL
  COMMENT "link headers..."
)

macro(makeLink src dest target)
  add_custom_command(
    TARGET ${target} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${src} ${dest}
    DEPENDS ${dest}
  )
endmacro()

add_custom_command(
  TARGET LINK_HEADERS PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${INC_DIR}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${LIB_DIR}
)

foreach(header_file ${INCLUDE_HEADERS})
  string(REPLACE "/" ";" arr ${header_file})
  list(GET arr -1 file_name)
  makeLink(${PROJECT_SOURCE_DIR}/${header_file} ${INC_DIR}/${file_name} LINK_HEADERS)
endforeach()

####TEST

#find_package(GTest REQUIRED CONFIG HINTS third_party/googletest)
find_package(GTest REQUIRED)

include_directories(${INC_DIR})
add_executable(${TEST_NAME} ${TEST_SRCS})

target_link_libraries(${TEST_NAME} GTest::Main)
# target_link_libraries(${TEST_NAME} gtest gtest_main pthread)
add_dependencies(${TEST_NAME} LINK_HEADERS)
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

####CONFIG

include(CMakePackageConfigHelpers)
set(CONFIG_INC_DIR ${INC_DIR})
set(CONFIG_LIB_DIR ${LIB_DIR})
set(CONFIG_BIN_DIR ${BIN_DIR})
configure_package_config_file(
  ${PROJECT_NAME}-config.cmake.in
  ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}-config.cmake
  INSTALL_DESTINATION ${CMAKE_CONFIG_INSTALL_DIR}
  PATH_VARS CONFIG_INC_DIR CONFIG_LIB_DIR CONFIG_BIN_DIR
)

set(CONFIG_INC_DIR ${CMAKE_INSTALL_INCLUDEDIR})
set(CONFIG_LIB_DIR ${CMAKE_INSTALL_LIBDIR})
set(CONFIG_BIN_DIR ${CMAKE_INSTALL_BINDIR})
configure_package_config_file(
  ${PROJECT_NAME}-config.cmake.in
  ${CMAKE_CONFIG_INSTALL_FILE}
  INSTALL_DESTINATION ${CMAKE_CONFIG_INSTALL_DIR}
  PATH_VARS CONFIG_INC_DIR CONFIG_LIB_DIR CONFIG_BIN_DIR
)

install(
  FILES ${CMAKE_CONFIG_INSTALL_FILE}
  DESTINATION ${CMAKE_CONFIG_INSTALL_DIR}
  COMPONENT devel
  RENAME ${PROJECT_NAME}-config.cmake
)

#### PACK

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "gtl")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

execute_process(
  COMMAND git log -n1 --format=%ad --date=short
  RESULT_VARIABLE GIT_LOG_RET
  OUTPUT_VARIABLE GIT_YMD
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (NOT GIT_LOG_RET EQUAL 0)
  set(GIT_YMD 0)
endif()

string(REPLACE "-" "" RELEASE_LABEL ${GIT_YMD})
set(CPACK_RPM_PACKAGE_RELEASE ${RELEASE_LABEL})

set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL devel)
set(CPACK_RPM_FILE_NAME RPM-DEFAULT)
set(CPACK_RPM_PACKAGE_RELEASE_DIST ON)

set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION /usr/lib64/cmake)

install(
  FILES ${INCLUDE_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
  COMPONENT devel
)

install(
  FILES README.md
  DESTINATION "${CMAKE_INSTALL_DOCDIR}-${PROJECT_VERSION}"
  COMPONENT devel
)

install(
  DIRECTORY docs
  DESTINATION "${CMAKE_INSTALL_DOCDIR}-${PROJECT_VERSION}"
  COMPONENT devel
)

set(CPACK_RPM_SPEC_MORE_DEFINE "%define __spec_install_post /bin/true") # disable strip
set(CPACK_GENERATOR "RPM")

include(CPack)
