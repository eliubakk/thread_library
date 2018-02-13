#ifndef _THREAD_IMPL_H
#define _THREAD_IMPL_H

#include "thread.h"
#include <ucontext.h>
#include <queue>

class thread::impl{
	public:
		impl(thread_startfunc_t, void *);
		~impl();
		ucontext_t* context;
		std::queue<thread::impl*> thread_join_queue;
		bool object_destroyed;
		char *stack;
		unsigned long id;

		static void thread_wrapper(thread_startfunc_t, void*);
};

#endif