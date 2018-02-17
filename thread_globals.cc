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

void swap(bool from_cpu, bool push_to_ready_queue){
	ucontext_t *from = nullptr, *to = nullptr;

	if(cpu::self()->impl_ptr->running_thread == nullptr && (!from_cpu || thread_ready_queue.empty()))
		//no thread to run, or interrupt handler was called from cpu.
		return;
	if(cpu::self()->impl_ptr->running_thread != nullptr && thread_ready_queue.empty() && push_to_ready_queue)
		//yield or interrupt but no other thread to run.
		return;

	if(from_cpu)
		from = cpu::self()->impl_ptr->context;
	else if(cpu::self()->impl_ptr->prev_thread == nullptr)
		from = cpu::self()->impl_ptr->running_thread->context;

	
	if(cpu::self()->impl_ptr->running_thread != nullptr && thread_ready_queue.empty()){
		//thread finished and no other thread to run.
		cpu::self()->impl_ptr->running_thread = nullptr;
		to = cpu::self()->impl_ptr->context;
	}
	else{
		//run next thread 
		if(push_to_ready_queue)
			thread_ready_queue_push(cpu::self()->impl_ptr->running_thread, true);
		cpu::self()->impl_ptr->running_thread = thread_ready_queue.front();
		thread_ready_queue.pop();
		to = cpu::self()->impl_ptr->running_thread->context;
	}

	if(from == nullptr)
		//thread just finished, no need to save context.
		setcontext(to);
	else
		swapcontext(from, to);

	//swap context just returned, check if last running thread needs to be deleted
	if(cpu::self()->impl_ptr->prev_thread){
		if(cpu::self()->impl_ptr->prev_thread->object_destroyed){
			delete cpu::self()->impl_ptr->prev_thread;
		}else{
			cpu::self()->impl_ptr->prev_thread->finished = true;
		}
		cpu::self()->impl_ptr->prev_thread = nullptr;
	}
}
