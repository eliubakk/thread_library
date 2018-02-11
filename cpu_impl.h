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
		thread::impl* running_thread;
		bool yielded;
		bool finished;
		static void ipi_handler();
		static void timer_handler();
		//static void service_next_thread();
};

#endif