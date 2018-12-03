//
// Created by cxh on 18-12-3.
//

#include "net.h"

namespace net{
Channel::Channel() {
	_prev = NULL;
	_next = NULL;
	_expireTime = 0;
}

void Channel::setId(uint32_t id) {
	_id = id;
}

uint32_t Channel::getId() const {//`const` add by cxh.
	return _id;
}

void Channel::setArgs(void *args) {
	_args = args;
}

void* Channel::getArgs() {
	return _args;
}

void Channel::setHandler(IPacketHandler *handler) {
	_handler = handler;
}

IPacketHandler* Channel::getHandler() {
	return _handler;
}

void Channel::setExpireTime(int64_t expireTime) {
	_expireTime = expireTime;
}
}