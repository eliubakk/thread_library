#ifndef _MUTEX_IMPL_H
#define _MUTEX_IMPL_H

#include "mutex.h"
#include "thread_impl.h"
#include <queue>

class mutex::impl{
	public:
		impl();
		~impl();
		unsigned long owner;
		std::queue<thread::impl*> lock_queue;
		bool status;

		void lock();
		void unlock();

		#define LOCKED 1
		#define UNLOCKED 0
};

#endif