#include "thread.h"
#include "thread_impl.h"
#include "thread_globals.h"
#include "cpu.h"
#include "cpu_impl.h"
#include <cassert>
#include <ucontext.h>
#include <iostream>
#include <stdexcept>

using namespace std;

thread::thread(thread_startfunc_t func, void *arg){
	impl_ptr = new impl(func, arg); 	
	thread_ready_queue_push(impl_ptr);
} // create a new thread

thread::~thread(){
	static bool first = true;
	if(!first){
		assert_interrupts_enabled();
		cpu::interrupt_disable();
	}
	while(guard.exchange(1)){}
	if(!impl_ptr->finished){
		impl_ptr->object_destroyed = true;
	}else{
		delete impl_ptr;
	}
	guard = 0;
	if(!first){
		assert_interrupts_disabled();
		cpu::interrupt_enable();
	}else{
		first = false;
	}
}

void thread::join(){
	assert_interrupts_enabled(); 
	cpu::interrupt_disable();
	while(guard.exchange(1)){}
	if (!impl_ptr->finished) {
		try{
			impl_ptr->thread_join_queue.push(cpu::self()->impl_ptr->running_thread);
		}catch(bad_alloc& e){
			guard = 0;
			assert_interrupts_disabled();
			cpu::interrupt_enable();
			throw e;
		}
		//printf("swap from join...");
		swap(false, false);
	}
	guard = 0;
	assert_interrupts_disabled();
	cpu::interrupt_enable();
}                        // wait for this thread to finish

void thread::yield(){
	assert_interrupts_enabled(); 
	cpu::interrupt_disable();
	while(guard.exchange(1)){}
	//printf("swap from yield...");
	swap(false, true);
	guard = 0;
	assert_interrupts_disabled();
	cpu::interrupt_enable();
}                // yield the CPU

/*
 * Move constructor and move assignment operator.  Implementing these is
 * optional in Project 2.
 
thread(thread&&);
thread& operator=(thread&&);*/
