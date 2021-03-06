//
// Created by cxh on 18-12-3.
//

#ifndef NET_NET_H
#define NET_NET_H
#include <unistd.h> //Standard symbolic constants and types
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>

#include <list>
#include <queue>
#include <vector>
#include <string>
#include <ext/hash_map>
#include "sys.h"

namespace net{
class TimeUtil;
class Thread;
class Runnable;
class DataBuffer;

class Packet;
class ControlPacket;
class IPacketFactory;
class IPacketHandler;
class IPacketStreamer;
class IServerAdapter;
class DefaultPacketStreamer;
class PacketQueue;

class Socket;
class ServerSocket;
class IOEvent;
class SocketEvent;
class EPollSocketEvent;
class Channel;
class ChannelPool;
class Connection;
class IOComponent;
class TCPAcceptor;
class TCPComponent;
class TCPConnection;
class Transport;
class UDPAcceptor;
class UDPComponent;
class UDPConnection;

class HttpRequestPacket;
class HttpResponsePacket;
class HttpPacketStreamer;
class DefaultHttpPacketFactory;
class PacketQueueThread;
class ConnectionManager;
}

#include "stats.h"

#include "packet.h"
#include "controlpacket.h"
#include "ipacketfactory.h"
#include "databuffer.h"
#include "ipackethandler.h"
#include "ipacketstreamer.h"
#include "iserveradapter.h"
#include "defaultpacketstreamer.h"
#include "packetqueue.h"

#include "socket.h"
#include "serversocket.h"
#include "socketevent.h"
#include "epollsocketevent.h"

#include "channel.h"
#include "channelpool.h"
#include "connection.h"
#include "tcpconnection.h"
#include "udpconnection.h"

#include "iocomponent.h"
#include "tcpacceptor.h"
#include "tcpcomponent.h"
#include "udpacceptor.h"
#include "udpcomponent.h"
#include "transport.h"

#include "httprequestpacket.h"
#include "httpresponsepacket.h"
#include "httppacketstreamer.h"
#include "packetqueuethread.h"
#include "connectionmanager.h"
#endif //COMMON_UTILS_NET_H
