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
	assert_interrupts_enabled(); cpu::interrupt_disable();
	while(guard.exchange(1)){}
	try{
		impl_ptr->waiting_queue.push(cpu::self()->impl_ptr->running_thread);
	}catch(bad_alloc& e){
		guard = 0;
		assert_interrupts_disabled();
		cpu::interrupt_enable();
		throw e;
	}
	try{
		m.impl_ptr->unlock();
	}catch(bad_alloc& e){
        guard = 0;
        assert_interrupts_disabled();
        cpu::interrupt_enable();
        throw e;
    }catch(runtime_error& e){
    	guard = 0;
    	assert_interrupts_disabled();
        cpu::interrupt_enable();
        throw e;
    }
    swap(false, false);
	try{
		m.impl_ptr->lock();
	}catch(bad_alloc& e){
        guard = 0;
        assert_interrupts_disabled();
        cpu::interrupt_enable();
        throw e;
    }
    guard = 0;
    assert_interrupts_disabled();
    cpu::interrupt_enable();
}                // wait on this condition variable

void cv::signal(){
    assert_interrupts_enabled();
    cpu::interrupt_disable();
    while(guard.exchange(1)){}
    try{
    	impl_ptr->wake_up();
    }catch(bad_alloc& e){
    	guard = 0;
    	assert_interrupts_disabled();
    	cpu::interrupt_enable();
    	throw e;
    }
	guard = 0;
	assert_interrupts_disabled();
   	cpu::interrupt_enable();
}                      // wake up one thread on this condition
                                        // variable
void cv::broadcast(){
    assert_interrupts_enabled(); 
    cpu::interrupt_disable();
    while(guard.exchange(1)){}
    try{
    	while(impl_ptr->wake_up());
    }catch(bad_alloc& e){
    	guard = 0;
    	assert_interrupts_disabled();
    	cpu::interrupt_enable();
    	throw e;
    }
	guard = 0;
	assert_interrupts_disabled();
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