#include "thread.h"
#include "thread_impl.h"
#include "thread_globals.h"
#include "cpu.h"
#include <cassert>
#include <ucontext.h>

using namespace std;

thread::thread(thread_startfunc_t func, void *arg){
	impl_ptr = new impl(); 
	char *stack = new char [STACK_SIZE];
	impl_ptr->context->uc_stack.ss_sp = stack;
	impl_ptr->context->uc_stack.ss_size = STACK_SIZE;
	impl_ptr->context->uc_stack.ss_flags = 0;
	impl_ptr->context->uc_link = nullptr;

	makecontext(impl_ptr->context, (void (*)())func, 1, arg);
	thread_ready_queue.push(impl_ptr);
	if (cpu_suspended_queue.empty()) {

	}
	else {
		cpu *curr_cpu = cpu_suspended_queue.front();
		cpu_suspended_queue.pop();
		curr_cpu->interrupt_send();
	}

} // create a new thread
thread::~thread(){
	assert(false);
}

void thread::join(){
	assert(false);
}                        // wait for this thread to finish

void thread::yield(){
	thread_ready_queue.push(impl_ptr);
	swapcontext(impl_ptr->context, impl_ptr->context->uc_link);
}                // yield the CPU

/*
 * Move constructor and move assignment operator.  Implementing these is
 * optional in Project 2.
 
thread(thread&&);
thread& operator=(thread&&);*/
