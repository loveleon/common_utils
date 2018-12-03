//
// Created by cxh on 18-12-3.
//

//#include "channelpool.h"
#include "net.h"
namespace net{
atomic_t ChannelPool::_globalChannelId = {1};
atomic_t ChannelPool::_globalTotalCount={0};

ChannelPool::ChannelPool() {
	_freeListHead = NULL;
	_freeListTail = NULL;
	_useListHead  = NULL;
	_useListTail  = NULL;
	_maxUseCount = 0;//impl,`_maxUseCount` default initialized.
}
ChannelPool::~ChannelPool() {
	std::list<Channel *>::iterator it = _clusterList.begin();
	for(;it != _clusterList.end();++it){
		delete [] *it;
	}
}

/*
	 * get a new channel
	 * @return a channel
	 */
Channel* ChannelPool::allocChannel() {

}
}
