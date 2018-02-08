#include "cv.h"
#include "cv_impl.h"
#include "cpu.h"
#include "cpu_impl.h"
#include <ucontext.h>
#include <cassert>

using namespace std;

cv::cv(){
    impl_ptr = new impl();
}

cv::~cv(){
	delete impl_ptr;
    //assert(false);
}

void cv::wait(mutex& m){
	m.unlock();
	cpu::interrupt_disable();
	while(guard.exchange(1)){}
    impl_ptr->waiting_queue.push(cpu::self()->impl_ptr->running_thread);
    swapcontext(cpu::self()->impl_ptr->running_thread->context,
    			cpu::self()->impl_ptr->context);
    guard = 0;
    cpu::interrupt_enable();
    m.lock();
}                // wait on this condition variable

void cv::signal(){
    cpu::interrupt_disable();
    while(guard.exchange(1)){}
    impl_ptr->wake_up();
	guard = 0;
   	cpu::interrupt_enable();
}                      // wake up one thread on this condition
                                        // variable
void cv::broadcast(){
    cpu::interrupt_disable();
    while(guard.exchange(1)){}
    while(impl_ptr->wake_up());
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