#ifndef _MUTEX_IMPL_H
#define _MUTEX_IMPL_H

#include "mutex.h"
#include "thread_globals.h"
#include "thread_impl.h"
#include <atomic>
#include <queue>

class mutex::impl{
	public:
		impl();
		~impl();
		thread::impl* owner;
		std::queue<thread::impl*> lock_queue;
		bool status;

		#define LOCKED 1
		#define UNLOCKED 0
};

#endif