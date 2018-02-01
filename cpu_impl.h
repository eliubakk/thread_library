#ifndef _CPU_IMPL_H
#define _CPU_IMPL_H

#include "cpu.h"
#include <ucontext.h>

class cpu::impl{
	public:
		impl();
		~impl();
		ucontext_t* context;
		bool yielded;
		ucontext_t* yielded_context;
};

#endif