//
// Created by cxh on 18-12-3.
//

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
        for(int i= 2;i<CHANNEL_CLUSTER_SIZE;i++){
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

	//generate channel id
	uint32_t id = atomic_add_return(1,&ChannelPool::_globalChannelId);
	id = (id & 0X0FFFFFFF);
	if(id == 0)
	{
		id = 1;
		atomic_set(&ChannelPool::_globalChannelId,1);
	}
	channel->_id = id;
	channel->_handler = NULL;
	channel->_args = NULL;

	//put 'channel' into hashmap
	_useMap[id] = channel;
	if(_maxUseCount < (int)_useMap.size())
	{
		_maxUseCount = _useMap.size();
	}
	_mutex.unlock();

	return channel;
}

/*
 * release a channel
 * @param channel:to be release channel.
 */
bool ChannelPool::freeChannel(net::Channel *channel) {
	__gnu_cxx::hash_map<uint32_t ,Channel*>::iterator it;
	_mutex.lock();

	it = _useMap.find(channel->_id);
	if(it == _useMap.end()){
		_mutex.unlock();
		return false;
	}

	//delete from map.
	_useMap.erase(it);

	//delete from _userList
	if(_useListHead == channel){//head
		_useListHead = _useListHead->_next;
	}
	if(_useListTail == channel){//tail
		_useListTail = channel->_prev;
	}
	if(channel->_prev != NULL){
		channel->_prev->_next = channel->_next;
	}
	if(channel->_next != NULL){
		channel->_next->_prev = channel->_prev;
	}

	//add 'channel' into free list.
	channel->_prev = _freeListTail;
	channel->_next = NULL;
	if(_freeListTail == NULL)
	{
		_freeListHead = channel;
	}else{
		_freeListTail->_next = channel;
	}
	_freeListTail = channel;
	channel->_id = 0;
	channel->_args = NULL;
	channel->_handler = NULL;
	_mutex.unlock();
	return true;
}

void ChannelPool::appendChannel(net::Channel *channel) {
	_mutex.lock();
	channel->_prev = _freeListTail;
	channel->_next = NULL;
	if(_freeListTail == NULL){
		_freeListHead = channel;
	}else{
		_freeListTail->_next = channel;
	}
	_freeListTail = channel;
	channel->_id = 0;
	channel->_args = NULL;
	channel->_handler = NULL;
	_mutex.unlock();
}

/*
 * get a Channel by channel id
 * @param id:channel id
 * @return Channel
 */
Channel* ChannelPool::offerChannel(uint32_t id) {
	Channel * channel = NULL;
	__gnu_cxx::hash_map<uint32_t , Channel *>::iterator it;

	_mutex.lock();
	it = _useMap.find(id);
	if(it != _useMap.end()){
		Channel * channel = it->second;
		channel = it -> second;

		//delete from _userList
		if(_useListHead == channel){//head
			_useListHead = _useListHead->_next;
		}
		if(_useListTail == channel) {//tail
			_useListTail = channel->_prev;
		}
		if(channel->_prev != NULL){
			channel->_prev->_next = channel->_next;
		}
		if(channel->_next != NULL){
			channel->_next->_prev = channel->_prev;
		}
		channel->_prev = NULL;
		channel->_next = NULL;//because delete action,so let's make pointer point to NULL;
	}
	_mutex.unlock();

	return channel;
}

/*
 * delete timeout channel'list from _useList, and delete it from hashmap.
 * @param now:right now timestamp.
 * @return
 */
Channel* ChannelPool::getTimeoutList(int64_t now) {
	Channel * list = NULL;

	_mutex.lock();
	if(_useListHead == NULL){
		_mutex.unlock();
		return list;
	}
	Channel * channel = _useListHead;
	while(channel != NULL){
		if(channel->_expireTime >= now){
			break;
		}
		_useMap.erase(channel->_id);
		channel = channel->_next;
	}

	// have timeout list
	if(channel != _useListHead){
		list = _useListHead;
		if(channel == NULL){//ALL TIMEOUT
			_useListHead = _useListTail = NULL;
		}else{
			if(channel->_prev != NULL){
				channel->_prev->_next = NULL;
			}
			channel->_prev = NULL;
			_useListTail = channel;
		}
	}

	_mutex.unlock();

	return list;
}

/*
 * add 'addList' into _freeList
 * @param addList:will to be added into _freeList.
 */
bool ChannelPool::appendFreeList(net::Channel *addList) {
	if(addList == NULL)
		return true;

	_mutex.lock();

	//find 'tail'
	Channel * tail = addList;
	while(tail->_next != NULL){
		tail->_id = 0;
		tail->_args = NULL;
		tail->_handler = NULL;
		tail = tail->_next;
	}
	tail->_id = 0;
	tail->_args = NULL;
	tail->_handler = NULL;

	//add into _freeList
	addList->_prev = _freeListTail;
	if(_freeListTail == NULL){
		_freeListHead = addList;
	}else{
		_freeListTail->next = addList;
	}
	_freeListTail = tail;

	_mutex.unlock();
	return true;
}

void ChannelPool::setExpireTime(net::Channel *channel, int64_t now) {
	_mutex.lock();
	if(channel != NULL){
		channel->_expireTime = now;
	}
	_mutex.unlock();
}
}//namespace net
