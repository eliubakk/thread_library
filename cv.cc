#include "cv.h"
#include "cv_impl.h"
#include "cpu.h"
#include "cpu_impl.h"
#include "mutex_impl.h"
#include <ucontext.h>
#include <cassert>
#include <stdexcept>

using namespace std;

cv::cv(){
    impl_ptr = new impl();
}

cv::~cv(){
	delete impl_ptr;
    //assert(false);
}

void cv::wait(mutex& m){
	cpu::interrupt_disable();
	while(guard.exchange(1)){}
	try{
		impl_ptr->waiting_queue.push(cpu::self()->impl_ptr->running_thread);
	}catch(bad_alloc& e){
		guard = 0;
		cpu::interrupt_enable();
		throw e;
	}
	try{
		m.impl_ptr->unlock();
	}catch(bad_alloc& e){
        guard = 0;
        cpu::interrupt_enable();
        throw e;
    }catch(runtime_error& e){
    	guard = 0;
        cpu::interrupt_enable();
        throw e;
    }
    if(!thread_ready_queue.empty() && cpu::self()->impl_ptr->running_thread != nullptr){
		swap_to_next_thread(false);
	}
	else{
		swapcontext(cpu::self()->impl_ptr->running_thread->context,
				cpu::self()->impl_ptr->context);
	}
	try{
		m.impl_ptr->lock();
	}catch(bad_alloc& e){
        guard = 0;
        cpu::interrupt_enable();
        throw e;
    }
    guard = 0;
    cpu::interrupt_enable();
}                // wait on this condition variable

void cv::signal(){
    cpu::interrupt_disable();
    while(guard.exchange(1)){}
    try{
    	impl_ptr->wake_up();
    }catch(bad_alloc& e){
    	guard = 0;
    	cpu::interrupt_enable();
    	throw e;
    }
	guard = 0;
   	cpu::interrupt_enable();
}                      // wake up one thread on this condition
                                        // variable
void cv::broadcast(){
    cpu::interrupt_disable();
    while(guard.exchange(1)){}
    try{
    	while(impl_ptr->wake_up());
    }catch(bad_alloc& e){
    	guard = 0;
    	cpu::interrupt_enable();
    	throw e;
    }
	guard = 0;
   	cpu::interrupt_enable();
}                   // wake up all threads on this condition
                                        // variable

//  * Move constructor and move assignment operator.  Implementing these is
//  * optional in Project 2.
 
cv::cv(cv&&){
    assert(false);
}

cv& cv::operator=(cv&&){
    assert(false);
}