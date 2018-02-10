#include "thread_globals.h"
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
	}catch(bad_alloc e){
		if(!have_guard){
			guard = 0;
			if(!first)
				cpu::interrupt_enable();
			else
				first = false;
		}
		throw bad_alloc("thread_ready_queue.push failed.");
	}


	if(!have_guard){
		guard = 0;
		if(!first)
			cpu::interrupt_enable();
		else
			first = false;
	}

}