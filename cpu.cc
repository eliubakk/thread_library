#include "cpu.h"
#include "cpu_impl.h"
#include "thread.h"
#include "thread_impl.h"
#include "thread_globals.h"
#include <ucontext.h>

using namespace std;

/*
 * cpu::init() initializes a CPU.  It is provided by the thread library
 * and called by the infrastructure.  After a CPU is initialized, it
 * should run user threads as they become available.  If func is not
 * nullptr, cpu::init() also creates a user thread that executes func(arg).
 *
 * On success, cpu::init() should not return to the caller.
 */
void cpu::init(thread_startfunc_t func, void *arg){
	impl_ptr = new impl();
	//TODO: add guard stuff
	while(1){
		if(func != nullptr){
			thread temp(func, arg);
			func = nullptr;
		}
		else if(!thread_ready_queue.empty()){
			thread::impl* curr_thread = thread_ready_queue.front();
			thread_ready_queue.pop();
			//curr_thread->context->uc_link = impl_ptr->context;
			swapcontext(impl_ptr->context, curr_thread->context);
			if(!impl_ptr->yielded){
				delete curr_thread;
			}else{
				delete curr_thread->context;
				curr_thread->context = impl_ptr->yielded_context;
				thread_ready_queue.push(curr_thread);
				impl_ptr->yielded = false;
			}
		}else{
			cpu_suspended_queue.push(this);
			cpu::interrupt_enable_suspend();
		}
	}
}