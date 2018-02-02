#include "thread.h"
#include "thread_impl.h"
#include "thread_globals.h"
#include "cpu.h"
#include "cpu_impl.h"
#include <cassert>
#include <ucontext.h>

using namespace std;

thread::thread(thread_startfunc_t func, void *arg){
	impl_ptr = new impl(func, arg); 
	thread_ready_queue_push(impl_ptr);
} // create a new thread

thread::~thread(){
	//assert(false);
}

void thread::join(){
	cpu::interrupt_disable();
	if (impl_ptr) {
		impl_ptr->thread_join_queue.push(cpu::self()->impl_ptr->running_thread);
		swapcontext(cpu::self()->impl_ptr->running_thread->context,
					cpu::self()->impl_ptr->context);
	}
	cpu::interrupt_enable();
}                        // wait for this thread to finish

void thread::yield(){
	cpu::interrupt_disable();
	cpu::self()->impl_ptr->yielded = true;
	swapcontext(cpu::self()->impl_ptr->running_thread->context,
				cpu::self()->impl_ptr->context);
	cpu::interrupt_enable();
}                // yield the CPU

/*
 * Move constructor and move assignment operator.  Implementing these is
 * optional in Project 2.
 
thread(thread&&);
thread& operator=(thread&&);*/
