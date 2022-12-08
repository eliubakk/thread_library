#include "cpu.h"
#include "cpu_impl.h"
#include "thread.h"
#include "thread_impl.h"
#include "thread_globals.h"
#include <ucontext.h>
#include <iostream>

using namespace std;

/*
 * cpu::init() initializes a CPU.  It is provided by the thread library
 * and called by the infrastructure.  After a CPU is initialized, it
 * should run user threads as they become available.  If func is not
 * nullptr, cpu::init() also creates a user thread that executes func(arg).
 *
 * On success, cpu::init() should not return to the caller.
 */
void cpu::init(thread_startfunc_t func, void *arg){
	impl_ptr = new impl();
	interrupt_vector_table[cpu::TIMER] = (interrupt_handler_t) cpu::impl::timer_handler;
	interrupt_vector_table[cpu::IPI] = (interrupt_handler_t) cpu::impl::ipi_handler;
	//TODO: add guard stuff
	if(func != nullptr){
		thread temp(func, arg);
		func = nullptr;
	}
	while(guard.exchange(1)){}
	while(1){
		//printf("swap from cpu...");
		swap(true, false);
		cpu_suspended_queue.push(this);
		guard = 0;
		assert_interrupts_disabled();
		cpu::interrupt_enable_suspend();
		assert_interrupts_enabled(); 
		cpu::interrupt_disable();
		while(guard.exchange(1)){}
	}
}