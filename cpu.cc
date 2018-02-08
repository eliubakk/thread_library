#include "cpu.h"
#include "cpu_impl.h"
#include "thread.h"
#include "thread_impl.h"
#include "thread_globals.h"
#include <ucontext.h>

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
		if(!thread_ready_queue.empty()){
			impl_ptr->running_thread = thread_ready_queue.front();
			thread_ready_queue.pop();
			if(!(cpu_suspended_queue.empty() ||
				 thread_ready_queue.empty())){
				cpu* wakeup_cpu = cpu_suspended_queue.front();
				cpu_suspended_queue.pop();
				wakeup_cpu->interrupt_send();
			}
			swapcontext(impl_ptr->context, impl_ptr->running_thread->context);
			if(impl_ptr->yielded){
				thread_ready_queue_push(impl_ptr->running_thread, true);
				impl_ptr->yielded = false;
			}else if(impl_ptr->finished){
				delete[] impl_ptr->running_thread->stack;
				delete impl_ptr->running_thread->context;
				impl_ptr->running_thread->context = nullptr;
				if(impl_ptr->running_thread->object_destroyed){
					delete impl_ptr->running_thread;
				}
				impl_ptr->finished = false;
			}
			impl_ptr->running_thread = nullptr;
		}else{
			cpu_suspended_queue.push(this);
			guard = 0;
			cpu::interrupt_enable_suspend();
			cpu::interrupt_disable();
			while(guard.exchange(1)){}
		}
	}
}