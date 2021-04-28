cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -pipe -std=c++11")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

project(gtl_test)

if (NOT "$ENV{LIBRARY_PATH}" STREQUAL "")
    string(REPLACE ":" ";" LIBRARY_PATH $ENV{LIBRARY_PATH})
    set(CMAKE_SYSTEM_LIBRARY_PATH ${LIBRARY_PATH};${CMAKE_SYSTEM_LIBRARY_PATH})
endif ()

if (NOT "$ENV{CPLUS_INCLUDE_PATH}" STREQUAL "")
    string(REPLACE ":" ";" INCLUDE_PATH $ENV{CPLUS_INCLUDE_PATH})
    set(CMAKE_SYSTEM_INCLUDE_PATH ${INCLUDE_PATH};${CMAKE_SYSTEM_INCLUDE_PATH})
endif ()

find_package(GTest REQUIRED)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

aux_source_directory(. SRCS)
link_directories(${GTest_LIB_DIR})
link_libraries(gtest)

add_executable(${PROJECT_NAME} ${SRCS})

add_custom_target(check COMMAND ./${PROJECT_NAME})
add_dependencies(check ${PROJECT_NAME})