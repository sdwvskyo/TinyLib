#ifndef CONDITION_H
#define CONDITION_H

#include <boost/noncopyable.hpp>
#include <pthread.h>

namespace Tiny
{

class MutexLock;

class Condition : boost::noncopyable
{
public:
	explicit Condition(MutexLock &mutex);
	~Condition();

	void wait();
	void notify();
	void notifyAll();
	
private:
	MutexLock &mutex_;
	pthread_cond_t cond_;
};

}


#endif	//CONDITION_H