//
// Created by cxh on 18-12-3.
//

#ifndef NET_CHANNELPOOL_H
#define NET_CHANNELPOOL_H
#include <backward/hash_map>
#define CHANNEL_CLUSTER_SIZE 25
namespace net{
class ChannelPool {
 public:
	//constructor.
	ChannelPool();

	//destructor.
	~ChannelPool();

	/*
	 * get a new channel
	 * @return a channel
	 */
	Channel * allocChannel();

	/*
	 * release a channel.
	 * @param channel: to be release channel*
	 * @return
	 */
	bool freeChannel(Channel *channel);
	void appendChannel(Channel *channel);

	/*
	 * look up a channel
	 * @param id:channel id
	 * @return Channel
	 */
	Channel * offerChannel(uint32_t id);

	/*
	 * from useList to find out timeout channel list,then delete it from hash_map.
	 * @param now:right now time
	 *
	 */
	Channel * getTimeoutList(int64_t now);

	/*
	 * add 'addList' to free list.
	 * @param addList:the list to be add to free list.
	 *
	 */
	bool appendFreeList(Channel * addList);

	/*
	 * the length of the used list's len.
	 */
	int getUseListCount(){
		return _useMap.size();
	}

	void setExpireTime(Channel * channel,int64_t now);

 private:
	__gnu_cxx::hash_map<uint32_t ,Channel*> _useMap;//map
	std::list<Channel*> _clusterList;				//cluster list
	sys::CThreadMutex _mutex;
	Channel * _freeListHead;						//empty list
	Channel * _freeListTail;
	Channel * _useListHead;							//be used list
	Channel * _useListTail;

	int _maxUseCount;								//be used count
	static atomic_t _globalChannelId;				//generate uniform id
	static atomic_t _globalTotalCount;
};
}


#endif //NET_CHANNELPOOL_H
