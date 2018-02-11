#include "cpu_impl.h"
#include "thread_globals.h"
#include <cassert>
#include <iostream>

using namespace std;

cpu::impl::impl(){
	context = new ucontext_t();
	assert(!getcontext(context));
	running_thread = nullptr;
	yielded = false;
	finished = false;
}

cpu::impl::~impl(){
	delete context;
}

void cpu::impl::ipi_handler(){
	//TODO: Decide wtf to do here
}

void cpu::impl::timer_handler(){
	cpu::interrupt_disable();
	while(guard.exchange(1)){}
	if(!thread_ready_queue.empty() && cpu::self()->impl_ptr->running_thread != nullptr){
		thread_ready_queue.push(cpu::self()->impl_ptr->running_thread);
		cpu::self()->impl_ptr->running_thread = thread_ready_queue.front();
		thread_ready_queue.pop();
		swapcontext(thread_ready_queue.back()->context,
					cpu::self()->impl_ptr->running_thread->context);
	}
	guard = 0;
	cpu::interrupt_enable();
}