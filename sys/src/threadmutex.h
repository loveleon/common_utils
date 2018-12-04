//
// Created by cxh on 18-12-4.
//

#ifndef SYS_THREADMUTEX_H
#define SYS_THREADMUTEX_H
namespace sys{
/*
 * linux thread lock
 */

/*
 *
 * linux thread lock --mutex
 */
class CThreadMutex{
 public:
	/*
	 * constructor
	 */
	CThreadMutex(){
		const int iRet = pthread_mutex_init(&_mutex,NULL);
		assert(iRet == 0);
	}

	/*
	 * destructor
	 */
	~CThreadMutex(){
		pthread_mutex_destroy(&_mutex);
	}

	/*
	 * get lock
	 */
	void lock(){
		pthread_mutex_lock(&_mutex);
	}

	/*
	 * trylock get lock
	 */
	int trylock(){
		return pthread_mutex_trylock(&_mutex);
	}

	/*
	 * release lock
	 */
	void unlock(){
		pthread_mutex_unlock(&_mutex);
	}
 protected:
	pthread_mutex_t _mutex;
};

/*
 * @brief thread Guard
 */
class CThreadGurad{
 public:
	CThreadGurad(CThreadMutex *mutex){
		if(mutex){
			_mutex = mutex;
			_mutex->lock();
		}
	}

	~CThreadGurad(){
		if(_mutex){
			_mutex->unlock();
		}
	}
 private:
	CThreadMutex * _mutex;
};
}
#endif //PROJECT_THREADMUTEX_H
