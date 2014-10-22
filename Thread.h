#ifndef THREAD_H
#define THREAD_H

#include <boost/noncopyable.hpp>
#include <functional>
#include <pthread.h>

namespace Tiny
{

class Thread : boost::noncopyable
{
public:
	typedef std::function<void ()> ThreadCallback;

	explicit Thread(const ThreadCallback &callback);
	~Thread();

	void start();
	void join();

	void setCallback(const ThreadCallback &callback) 
	{ callback_ = callback; }

private:
	static void *runInThread(void *arg);

	pthread_t threadId_;
	bool isRunning_;
	ThreadCallback callback_;	//回调函数

};

}

#endif	//THREAD_H