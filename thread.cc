#include "thread.h"
#include "thread_impl.h"
#include "thread_globals.h"
#include <cassert>

using namespace std;

thread::thread(thread_startfunc_t func, void *arg){
	impl_ptr = new impl(); 
	char *stack = new char [STACK_SIZE];
	impl_ptr->context->uc_stack.ss_sp = stack;
	impl_ptr->context->uc_stack.ss_size = STACK_SIZE;
	impl_ptr->context->uc_stack.ss_flags = 0;
	impl_ptr->context->uc_link = nullptr;

	makecontext(impl_ptr, func, 1, arg);
	thread_ready_queue.push(impl_ptr);

} // create a new thread
thread::~thread(){
	assert(false);
}

void thread::join(){
	assert(false);
}                        // wait for this thread to finish

void thread::yield(){
	assert(false);
}                // yield the CPU

/*
 * Move constructor and move assignment operator.  Implementing these is
 * optional in Project 2.
 
thread(thread&&);
thread& operator=(thread&&);*/
