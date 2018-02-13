#include "mutex_impl.h"
#include "cpu_impl.h"
#include <stdexcept>

using namespace std;

mutex::impl::impl(){
	owner = 0;
	status = UNLOCKED;
}

mutex::impl::~impl(){
	//TODO
}

void mutex::impl::lock(){
	if(status == UNLOCKED){
        status = LOCKED;
        owner = cpu::self()->impl_ptr->running_thread->id;
    }else{
        lock_queue.push(cpu::self()->impl_ptr->running_thread);
        swapcontext(cpu::self()->impl_ptr->running_thread->context,
                cpu::self()->impl_ptr->context);
    }
}

void mutex::impl::unlock(){
	if(owner != cpu::self()->impl_ptr->running_thread->id)
    	throw runtime_error("A thread tried to unlock a mutex it did not hold.");

    status = UNLOCKED;
    if(!lock_queue.empty()){
        status = LOCKED;
        try{
        	thread_ready_queue_push(lock_queue.front(), true);
        }catch(bad_alloc& e){
        	status = UNLOCKED;
        	throw e;
        }
        owner = lock_queue.front()->id;
        lock_queue.pop();
    }
}