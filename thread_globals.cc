#include "thread_globals.h"
#include "cpu_impl.h"
#include <ucontext.h>
#include <stdexcept>

using namespace std;

queue<thread::impl*> thread_ready_queue;
queue<cpu*> cpu_suspended_queue;

void thread_ready_queue_push(thread::impl* t, bool have_guard)
{
	static bool first = true;
	if(!have_guard){
		if(!first)
			cpu::interrupt_disable();
		while(guard.exchange(1)){}
	}

	try{
		thread_ready_queue.push(t);
		if (!cpu_suspended_queue.empty()) {
			cpu *curr_cpu = cpu_suspended_queue.front();
			cpu_suspended_queue.pop();
			curr_cpu->interrupt_send();
		}
	}catch(bad_alloc& e){
		if(!have_guard){
			guard = 0;
			if(!first)
				cpu::interrupt_enable();
			else
				first = false;
		}
		throw e;
	}


	if(!have_guard){
		guard = 0;
		if(!first)
			cpu::interrupt_enable();
		else
			first = false;
	}

}

void swap_to_next_thread(bool push_to_ready_queue){
	thread::impl* curr_t = cpu::self()->impl_ptr->running_thread;
	if(push_to_ready_queue)
		thread_ready_queue_push(curr_t, true);
	cpu::self()->impl_ptr->running_thread = thread_ready_queue.front();
	thread_ready_queue.pop();
	swapcontext(curr_t->context,
				cpu::self()->impl_ptr->running_thread->context);
}