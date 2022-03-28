cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 11)
if (WIN32)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  set(LINK_COMMAND copy_if_different)
else()
  add_compile_options(-Wall -fPIC -pipe -g)
  set(LINK_COMMAND create_symlink)
endif()
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

set(GTL_DIR ${PROJECT_SOURCE_DIR}/src/gtl)
# set(GTL_EXAMPLES_DIR ${PROJECT_SOURCE_DIR}/examples)
set(INC_DIR ${PROJECT_SOURCE_DIR}/ CACHE PATH "gtl inc")
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib CACHE PATH "gtl lib")
set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin CACHE PATH "gtl bin")

include(GNUInstallDirs)

set(CMAKE_CONFIG_INSTALL_FILE ${PROJECT_BINARY_DIR}/config.toinstall.cmake)
set(CMAKE_CONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR})

set(INCLUDE_HEADERS
  ${GTL_DIR}/algorithm/algorithm.h
  ${GTL_DIR}/algorithm/heap_op.h
  ${GTL_DIR}/algorithm/seq_op.h
  ${GTL_DIR}/algorithm/memory_op.h
  ${GTL_DIR}/container/array.h
  ${GTL_DIR}/container/compressed_pair.h
  ${GTL_DIR}/container/deque.h
  ${GTL_DIR}/container/list.h
  ${GTL_DIR}/container/list_base.h
  ${GTL_DIR}/container/priority_queue.h
  ${GTL_DIR}/container/queue.h
  ${GTL_DIR}/container/slist.h
  ${GTL_DIR}/container/slist_base.h
  ${GTL_DIR}/container/stack.h
  ${GTL_DIR}/container/storage.h
  ${GTL_DIR}/container/vector.h
  ${GTL_DIR}/container/hash_table.h
  ${GTL_DIR}/container/lru_cache.h
  ${GTL_DIR}/iterator/iterator_op.h
  ${GTL_DIR}/iterator/iterator.h
  ${GTL_DIR}/iterator/reverse_iterator.h
  ${GTL_DIR}/memory/smart_pointers.h
  ${GTL_DIR}/strings/string.h
  ${GTL_DIR}/strings/string_view.h
  ${GTL_DIR}/test/test_class.h
  ${GTL_DIR}/test/cow_string.h
  ${GTL_DIR}/util/common.h
)
aux_source_directory(${GTL_DIR}/container/test CONTAINER_TEST_SRCS)
aux_source_directory(${GTL_DIR}/algorithm/test ALGORITHM_TEST_SRCS)
aux_source_directory(${GTL_DIR}/memory/test MEMORY_TEST_SRCS)
aux_source_directory(${GTL_DIR}/strings/test STRINGS_TEST_SRCS)
aux_source_directory(${GTL_DIR}/test OTHER_TEST_SRCS)
set(TEST_SRCS
  ${CONTAINER_TEST_SRCS}
  ${ALGORITHM_TEST_SRCS}
  ${MEMORY_TEST_SRCS}
  ${STRINGS_TEST_SRCS}
  ${OTHER_TEST_SRCS}
)

add_custom_target(
  MKDIRS ALL
  COMMENT "mkdir..."
)

add_custom_command(
  TARGET MKDIRS PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${LIB_DIR}
)

####TEST

# find_package(GTest REQUIRED CONFIG HINTS third_party/googletest)
# find_package(GTest REQUIRED)
add_subdirectory(third_party/googletest)

if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()


include_directories(${PROJECT_SOURCE_DIR}/src)
add_executable(${TEST_NAME} ${TEST_SRCS})

target_link_libraries(${TEST_NAME} GTest::gtest_main spdlog::spdlog)
# target_link_libraries(${TEST_NAME} gtest gtest_main pthread)
add_dependencies(${TEST_NAME} MKDIRS)
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
  DIRECTORY ${GTL_DIR}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
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
