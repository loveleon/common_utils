//
// Created by cxh on 18-12-3.
//

#ifndef SYS_SYS_H
#define SYS_SYS_H
#include <assert.h>
#include <errno.h>

#include <cassert>
#include <iostream>
#include <sstream>
#include <pthread.h>
#include <vector>
#include <string>

#ifndef UNUSED
#define UNUSED(v) ((void)(v))
#endif

namespace sys{
class CConfig;
class CFileUtil;
class CStringUtil;
class CNetUtil;
class CTimeUtil;
class CProcess;
class CLogger;
class CThread;
class CThreadMutex;
class CThreadCond;
class Runnable;
class CDefaultRunnable;
class CFileQueue;
class IQueueHandler;
class CQueueThread;
class CFileQueueThread;
}

namespace util{
class nocopyable{
 protected:
	nocopyable();
	~nocopyable();
 private:
	const nocopyable& operator=(const nocopyable& );//const is necessary?
	nocopyable(const nocopyable&);
};

#if defined(__BCPLUSPLUS__) || defined(_MSC_VER)
	typedef __int64 Int64;
#define T_INT64(n) n##i64;
#elif defined(TNET_64)
	typedef long Int64;
#define T_INT64(n) n##L
#else
	typedef long long Int64;
#define T_INT64(n) n##LL
#endif

typedef unsigned char Byte;
typedef short Short;
typedef int Int;
typedef Int64 Long;
typedef float Float;
typedef double Double;

typedef ::std::vector<bool> BoolSeq;

typedef ::std::vector< Byte> ByteSeq;

typedef ::std::vector< Short> ShortSeq;

typedef ::std::vector< Int> IntSeq;

typedef ::std::vector< Long> LongSeq;

typedef ::std::vector< Float> FloatSeq;

typedef ::std::vector< Double> DoubleSeq;

typedef ::std::vector< ::std::string> StringSeq;

inline int getSystemErrno(){
	return errno;
}
}//namespace util

#include "atomic.h"
#include "config.h"
#include "fileutil.h"
#include "stringutil.h"
#include "tbnetutil.h"
#include "tbtimeutil.h"
#include "process.h"
#include "tblog.h"
#include "tbrwlock.h"

#include "runnable.h"
#include "iqueuehandler.h"
#include "defaultrunnable.h"
#include "thread.h"
#include "threadmutex.h"
#include "threadcond.h"

#include "queuethread.h"
#include "filequeue.h"
#include "filequeuethread.h"
#include "profiler.h"
#include "bytebuffer.h"
#endif //COMMON_UTILS_SYS_H
