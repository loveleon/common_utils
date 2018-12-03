//
// Created by cxh on 18-12-3.
//

#ifndef NET_CHANNEL_H
#define NET_CHANNEL_H
//#include <cstdint>
//#include "ipackethandler.h"
namespace net{
class Channel {
	friend class ChannelPool;
 public:
	//constructor.
	Channel();

	void setId(uint32_t id);

	uint32_t getId()const; // modify by cxh

	//set huichuan parameters
	void setArgs(void * args);

	//get args
	void * getArgs();

	//set IPacketHandler's handle.
	void setHandler(IPacketHandler * handler);

	//get IPacketHandler's handle.
	IPacketHandler * getHandler();

	//set expired time.
	void setExpireTime(int64_t expireTime);

	//get expired time.
	int64_t getExpireTime(){
		return _expireTime;
	}

	//next
	Channel* getNext(){
		return _next;
	}

 private:
	uint32_t _id;//channel id
	void * _args;// back parameters
	IPacketHandler* _handler;//
	int64_t _expireTime;//dao qi time
 private:
	Channel * _prev;// yong zai lian biao.
	Channel * _next;
};
}


#endif //COMMON_UTILS_CHANNEL_H
