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
	Channel * channel = NULL;

	_mutex.lock();
	if(_freeListHead == NULL){
	    assert(CHANNEL_CLUSTER_SIZE > 2);
	    Channel * channelCluster = new Channel[CHANNEL_CLUSTER_SIZE];
	    TBSYS_LOG(DEBUG,"allocate channel total numbers:%d (%d)",atomic_add_return(CHANNEL_CLUSTER_SIZE,&_globalTotalCount),sizeof(Channel));
	    _clusterList.push_back(channelCluster);
	    _freeListHead = _freeListTail = &channelCluster[1];//why set index=1??
        for(int = 2;i<CHANNEL_CLUSTER_SIZE;i++){
            _freeListTail->_next = channelCluster[i];
            channelCluster[i]._prev = _freeListTail;
            _freeListTail = _freeListTail->_next;
        }
        _freeListHead->_prev = NULL;
        _freeListTail->_next = NULL;
        channel = &channelCluster[0];// first item directly to use ,don't put into freelist.
	}else{
	    //take item from list head
        channel = _freeListHead;
        if(_freeListHead != NULL)
        {
            _freeListHead->_prev = NULL;
        }else{
            _freeListTail = NULL;   //don't understand!!!
        }
	}

	//put channel into used list.
    channel->_prev = _useListTail;
	channel->_next = NULL;
	channel->_expireTime =  (1ll<<62)+1;//TBNET_MAX_TIME + 1;//TBNET_MAX_TIME (1ll<<62)
        if(_useListTail ==NULL){
            _useListHead = channel;
        }else{
            _useListTail->_next = channel;
        }
        _useListTail = channel;
}
}
