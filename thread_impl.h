#ifndef _THREAD_IMPL_H
#define _THREAD_IMPL_H

#include "thread.h"
#include <ucontext.h>

class thread::impl{
	public:
		impl();
		ucontext_t* context;
};

#endif