cmake_minimum_required(VERSION 3.2)
project(common_utils)

#project(${PACKAGE_NAME} CXX C)

OPTION(BUILD_SHARED_LIBS
        "If enabled,build common_utils as a shared library.\
just add another line."
        OFF
        )
set(CMAKE_CXX_STANDARD 11)
include_directories(net)

SET(COMMON_UTILS_CXX_FALGS "-std=c++11" CACHE STRING "c++ std flag(default:c++11)")

#include current path.
list(APPEND COMMON_UTILS_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(net)
add_subdirectory(sys)