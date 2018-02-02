#ifndef _CPU_IMPL_H
#define _CPU_IMPL_H

#include "cpu.h"
#include "thread_impl.h"
#include <ucontext.h>

class cpu::impl{
	public:
		impl();
		~impl();
		ucontext_t* context;
		ucontext_t* join_context;
		thread::impl* thread_to_join;
		ucontext_t* yielded_context;
};

#endif