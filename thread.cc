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
	thread_ready_queue.push(impl_ptr);
	if (!cpu_suspended_queue.empty()) {
		cpu *curr_cpu = cpu_suspended_queue.front();
		cpu_suspended_queue.pop();
		curr_cpu->interrupt_send();
	}

} // create a new thread
thread::~thread(){
	//assert(false);
}

void thread::join(){
	if (impl_ptr) {
		ucontext_t * context = new ucontext_t();
		getcontext(context);
		cpu::self()->impl_ptr->joined_context = context;
		swapcontext(context, cpu::self()->impl_ptr->context);
	}
}                        // wait for this thread to finish

void thread::yield(){
	//thread_ready_queue.push(impl_ptr);
	ucontext_t * context = new ucontext_t();
	getcontext(context);
	cpu::self()->impl_ptr->yielded_context = context;
	swapcontext(context, cpu::self()->impl_ptr->context);
}                // yield the CPU

/*
 * Move constructor and move assignment operator.  Implementing these is
 * optional in Project 2.
 
thread(thread&&);
thread& operator=(thread&&);*/
