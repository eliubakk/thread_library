#include "thread_globals.h"

using namespace std;

queue<thread::impl*> thread_ready_queue;
queue<cpu*> cpu_suspended_queue;

void thread_ready_queue_push(thread::impl* t, bool have_guard)
{
	if(!have_guard){
		while(guard.exchange(1)){}
	}

	thread_ready_queue.push(t);
	if (!cpu_suspended_queue.empty()) {
		cpu *curr_cpu = cpu_suspended_queue.front();
		cpu_suspended_queue.pop();
		curr_cpu->interrupt_send();
	}
	if(!have_guard)
		guard = 0;
}