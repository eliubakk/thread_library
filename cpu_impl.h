#ifndef _CPU_IMPL_H
#define _CPU_IMPL_H

#include "cpu.h"
#include "thread_impl.h"
#include <ucontext.h>
#include "thread_globals.h"

class cpu::impl{
	public:
		impl();
		~impl();
		ucontext_t* context;
		ucontext_t* joined_context;
		thread::impl* thread_to_join;
		ucontext_t* yielded_context;
		static void interrupt_ipi_handler();
		static void interrupt_timer_handler();
};

#endif