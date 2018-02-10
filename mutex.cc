#include "mutex.h"
#include "mutex_impl.h"
#include "cpu.h"
#include "cpu_impl.h"
#include "thread_globals.h"
#include <cassert>
#include <stdexcept>

using namespace std;

mutex::mutex(){
    impl_ptr = new impl();
}

mutex::~mutex(){
    delete impl_ptr;
}

void mutex::lock(){
    cpu::interrupt_disable();
    while(guard.exchange(1)){}
    if(impl_ptr->status == UNLOCKED){
    	impl_ptr->status = LOCKED;
    	impl_ptr->owner = cpu::self()->impl_ptr->running_thread;
    }else{
    	impl_ptr->lock_queue.push(cpu::self()->impl_ptr->running_thread);
    	swapcontext(cpu::self()->impl_ptr->running_thread->context,
				cpu::self()->impl_ptr->context);
    }
    guard = 0;
    cpu::interrupt_enable();
}

void mutex::unlock(){
    cpu::interrupt_disable();
    while(guard.exchange(1)){}
    if(impl_ptr->owner != cpu::self()->impl_ptr->running_thread)
    	throw runtime_error("A thread tried to unlock a mutex it did not hold.");
    
    impl_ptr->status = UNLOCKED;
    if(!impl_ptr->lock_queue.empty()){
    	impl_ptr->status = LOCKED;
    	impl_ptr->owner = impl_ptr->lock_queue.front();
    	impl_ptr->lock_queue.pop();
        assert_interrupts_disabled();
    	thread_ready_queue_push(impl_ptr->owner, true);
    }
    guard = 0;
    cpu::interrupt_enable();
}


//  * Move constructor and move assignment operator.  Implementing these is
//  * optional in Project 2.
 
// mutex(mutex&&);
// mutex& operator=(mutex&&);