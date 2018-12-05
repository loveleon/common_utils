//
// Created by cxh on 18-12-4.
//

#include "net.h"

namespace net{
/*
 * constructor
 */
Connection::Connection(net::Socket *socket, net::IPacketStreamer *streamer, net::IServerAdapter *serverAdapter) {
	_socket = socket;
	_streamer = streamer;
	_serverAdapter = serverAdapter;
	_defaultPacketHandler = NULL;
	_iocomponent = NULL;
	_queueTimeout = 5000;
	_queueLimit = 50;
	_queueTotalSize = 0;
	//what about '_inputQueue' and '_outputQueue'
}

Connection::~Connection() {
	disconnect();
	_socket = NULL;
	_iocomponent = NULL;
}

/*
 * disconnect happen,make all queue packet timeout
 */
void Connection::disconnect() {
	_outputCond.lock();
	_myQueue.moveTo(&_outputQueue);
	_outputCond.unlock();
	checkTimeout(TBNET_MAX_TIME);//iocomponent.h
}

/*
 * send packet to output queue
 */
bool Connection::postPacket(net::Packet *packet, net::IPacketHandler *packetHandler, void *args, bool noblocking) {
	if(!isConnectState()){//when connection is not ready,we will do this action judgement??
		if(_iocomponent == NULL || _iocomponent->isAutoReconn() == false){
			return false;
		}else if(_outputQueue.size() > 10){
			return false;
		}else{
			TCPComponent * ioc = dynamic_cast<TCPComponent *>(_iocomponent);
			bool ret = false;
			if(ioc != NULL){
				_outputCond.lock();
				ret = ioc->init(false);
				_outputCond.unlock();
			}
			if(!ret)
				return false;
		}
	}

	//if it's client endpoint, and it have queue length limit.
	_outputCond.lock();
	_queueTotalSize = _outputQueue.size() + _channelPool.getUseListCount() + _myQueue.size();
	if(!_isServer && _queueLimit > 0 && noblocking && _queueTotalSize >= _queueLimit){
		_outputCond.unlock();
		return false;
	}
	_outputCond.unlock();
	Channel * channel = NULL;
	packet->setExpireTime(_queueTimeout);
	if(_streamer->existPacketHeader()){
		uint32_t chid = packet->getChannelId();
		if(_isServer){
			assert(chid > 0);
		}else{
			channel = _channelPool.allocChannel();

			//channel don't get
			if(channel == NULL){
				TBSYS_LOG(WARN,"allocate channel get error, id: %u",chid);
				return false;
			}

			channel->setHandler(packetHandler);
			channel->setArgs(args);
			packet->setChannel(channel);// set back
		}
	}
	_outputCond.lock();
	//write into output queue
	_outputQueue.push(packet);
	if(_iocomponent != NULL && _outputQueue.size() == 1U){
		_iocomponent->enableWrite(true);
	}
	_outputCond.unlock();
	if(!_isServer && _queueLimit > 0){
		_outputCond.lock();
		_queueTotalSize = _outputQueue.size() + _myQueue.size() + _channelPool.getUseListCount();
		if((_queueTotalSize > _queueLimit) && (noblocking == false)){
			bool * stop = NULL;
			if(_iocomponent && _iocomponent->getOwner()){
				stop = _iocomponent->getOwner()->getStop();
			}
			while(_queueTotalSize > _queueLimit && stop && *stop == false){
				if(_outputCond.wait(1000) == false){
					if(!isConnectState()){
						break;
					}
					_queueTotalSize = _outputQueue.size() + _channelPool.getUseListCount() + _myQueue.size();
				}

			}
		}
		_outputCond.unlock();
	}

	if(_isServer && _iocomponent){
		_iocomponent->subRef();
	}

	return true;
}

bool Connection::handPacket(net::DataBuffer *input, PacketHeader *header) {
	Packet * packet;
	IPacketHandler::HPRetCode rc;
	void * args = NULL;
	Channel * channel = NULL;
	IPacketHandler * packetHandler = NULL;

	if(_streamer->existPacketHeader() && !_isServer){
		uint32_t chid = header->_chid;				//get from header
		channel = _channelPool.offerChannel(chid);

		//channel don't get
		if(channel == NULL){
			input->drainData(header->_dataLen);
			TBSYS_LOG(WARN,"don't find channel, id: %u, %s", chid,sys::CNetUtil::addrToString(getServerId()).c_str());
			return false;
		}
		packetHandler = channel->getHandler();
		args = channel->getArgs();
	}

	//decode
	packet = _streamer->decode(input,header);
	if(packet == NULL){
		packet = &ControlPacket::BadPacket;
	}else{
		packet->setPacketHeader(header);
		//if batch push ,directly put into queue,return
		if(_isServer && _serverAdapter->_bathcPushPacket){
			_inputQueue.push(packet);
			if(_inputQueue.size() >= 15){
				_serverAdapter->handleBatchPacket(this,_inputQueue);
				_inputQueue.clear();
			}
			return true;
		}
	}

	//call handler.
	if(_isServer){
		if(_iocomponent) _iocomponent->addRef();
		rc = _serverAdapter->handlePacket(this, packet);
	}else{
		if(packetHandler == NULL) {//USE DEFAULT
			packetHandler = _defaultPacketHandler;
		}
		assert(packetHandler != NULL);

		rc = packetHandler->handlePacket(packet,args);
		channel->setArgs(NULL);// why NULL??
		//get channel back ,and release it.
		if(channel){
			_channelPool.appendChannel(channel);
		}
	}

	return true;
}
}
