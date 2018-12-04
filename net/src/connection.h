//
// Created by cxh on 18-12-4.
//

#ifndef NET_CONNECTION_H
#define NET_CONNECTION_H
#define READ_WRITE_SIZE 8192
#ifndef UNUSED
#define UNUSED(v) ((void)(v))
#endif

namespace net{
class Connection {
 public:
	/*
	 * constructor
	 */
	Connection(Socket * socket, IPacketStreamer * streamer, IServerAdapter * serverAdapter);

	/*
	 * destructor
	 */
	~Connection();

	/*
	 * set to be a server endpoint.
	 */
	void setServer(bool isServer){
		_isServer = isServer;
	}

	void setIOComponent(IOComponent * ioc){
		_iocomponent = ioc;
	}

	IOComponent * getIOComponent(){
		return _iocomponent;
	}

	/*
	 * set default  packet handler
	 */
	void setDefaultHandler(IPacketHandler * defaulthandler){
	    _defaultPacketHandler = defaulthandler;
	}

	/*
	 * send packet to output queue.
	 * @param packet:data package
	 * @param packetHandler : packet handler
	 * @param args : parameter
	 * @param noblocking: no blocking
	 */
	bool postPacket(Packet * packet, IPacketHandler * packetHandler=NULL, void * args=NULL , bool noblocking = true);

	/*
	 * when receive packet ,deal with function
	 */
	bool handPacket(DataBuffer * buffer, PacketHeader * header);

	/*
	 * check timeout
	 */
	bool checkTimeout(int64_t now);

	/*
	 * write to data??
	 */
	virtual void writeData() = 0;

	/*
	 * read  from data
	 */
	virtual void readData() = 0;
 protected:
	IPacketHandler * _defaultPacketHandler; // connection's default packet handler.
	bool _isServer; 						// is Server endpoint.
	IOComponent * _iocomponent;
	Socket * _socket;						//Socket handle.
	IPacketStreamer * _streamer;			//Packet analysis
	IServerAdapter * _serverAdapter;		//server adapter.

	PacketQueue _outputQueue;				//send queue.
	PacketQueue _inputQueue;				//receive queue.
	PacketQueue _myQueue;					// in write action, for temp using.
	sys::CThreadCond * _outputCond;			// send queue's cond variable.
	ChannelPool * _channelPool;				//channel pool
	int _queueTimeout;						//queue timeout
	int _queueTotalSize;					//queue total length.
	int _queueLimit;						//queue limit(max length) length,when big than it posting in will be waiting.
};
}


#endif //PROJECT_CONNECTION_H
