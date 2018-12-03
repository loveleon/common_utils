#include_directories(.)
include_directories("${PROJECT_SOURCE_DIR}/sys/src")
set(SRCS config.cpp)
add_library(sys ${SRCS})