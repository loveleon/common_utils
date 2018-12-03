#include_directories("${PROJECT_SOURCE_DIR}/net/src")
#add_library(net channel.cpp
#        channelpool.cpp
#        )
#target_sources(net
#        channelpool.cpp
#        channel.cpp
#        )
include_directories(.)
include_directories("${PROJECT_SOURCE_DIR}/sys/src")
include_directories(../../net/src)
set(SRCS channelpool.cpp
        channel.cpp
        ipacketfactory.cpp
        ipackethandler.cpp
        )
add_library(net ${SRCS})
target_link_libraries(net $SRCS)