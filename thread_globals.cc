#include "thread_globals.h"
#include "cpu_impl.h"
#include <ucontext.h>
#include <stdexcept>
#include <iostream>

using namespace std;

queue<thread::impl*> thread_ready_queue;
queue<cpu*> cpu_suspended_queue;

void thread_ready_queue_push(thread::impl* t, bool have_guard)
{
	static bool first = true;
	if(!have_guard){
		if(!first){
			assert_interrupts_enabled(); 
			cpu::interrupt_disable();
		}
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
			if(!first){
				assert_interrupts_disabled();
				cpu::interrupt_enable();
			}
			else
				first = false;
		}
		throw e;
	}


	if(!have_guard){
		guard = 0;
		if(!first){
			assert_interrupts_disabled();
			cpu::interrupt_enable();
		}
		else
			first = false;
	}

}

void check_finished(){
	if(cpu::self()->impl_ptr->prev_thread){
		//printf("prev thread finished, deleting...");
		if(cpu::self()->impl_ptr->prev_thread->object_destroyed){
			delete cpu::self()->impl_ptr->prev_thread;
		}else{
			cpu::self()->impl_ptr->prev_thread->finished = true;
		}
		cpu::self()->impl_ptr->prev_thread = nullptr;
	}
}

void swap(bool from_cpu, bool push_to_ready_queue){
	ucontext_t *from = nullptr, *to = nullptr;

	if(cpu::self()->impl_ptr->running_thread == nullptr && (!from_cpu || thread_ready_queue.empty())){
		//printf("queue empty, (from interrupt or cpu), returning.\n");
		//no thread to run, or interrupt handler was called from cpu.
		return;
	}

	if(!from_cpu && thread_ready_queue.empty() && push_to_ready_queue){
		//printf("interrupt or yield, nothing else to run, returning.\n");
		//yield or interrupt but no other thread to run.
		return;
	}

	if(from_cpu){
		//printf("from cpu...");
		from = cpu::self()->impl_ptr->context;
	}
	else if(cpu::self()->impl_ptr->prev_thread == nullptr){
		//printf("context needs to be saved...");
		from = cpu::self()->impl_ptr->running_thread->context;
	}else{
		//printf("thread finished, don't save context...");
	}

	
	if(!from_cpu && thread_ready_queue.empty()){
		//printf("nothing else to run, go to sleep...");
		//thread finished and no other thread to run.
		cpu::self()->impl_ptr->running_thread = nullptr;
		to = cpu::self()->impl_ptr->context;
	}
	else{
		//printf("run next thread...");
		//run next thread 
		if(push_to_ready_queue){
			//printf("from yield or interrupt...");
			thread_ready_queue_push(cpu::self()->impl_ptr->running_thread, true);
		}
		cpu::self()->impl_ptr->running_thread = thread_ready_queue.front();
		thread_ready_queue.pop();
		to = cpu::self()->impl_ptr->running_thread->context;
	}

	if(from == nullptr){
		//printf("setting context\n");
		//thread just finished, no need to save context.
		setcontext(to);
	}
	else{
		//printf("swapping context\n");
		swapcontext(from, to);
	}

	//printf("returned from swapcontext...");
	//swap context just returned, check if last running thread needs to be deleted
	check_finished();
	//printf("continuing\n");
}
