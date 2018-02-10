#include "mutex_impl.h"

using namespace std;

mutex::impl::impl(){
	owner = nullptr;
	status = UNLOCKED;
}

mutex::impl::~impl(){
	//TODO
}

mutex::impl::lock(){
	if(status == UNLOCKED){
        status = LOCKED;
        owner = cpu::self()->impl_ptr->running_thread;
    }else{
        lock_queue.push(cpu::self()->impl_ptr->running_thread);
        swapcontext(cpu::self()->impl_ptr->running_thread->context,
                cpu::self()->impl_ptr->context);
    }
}

mutex::impl::unlock(){
	if(owner != cpu::self()->impl_ptr->running_thread)
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
        owner = lock_queue.front();
        lock_queue.pop();
    }
}