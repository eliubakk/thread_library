#include "cpu_impl.h"
#include "thread_globals.h"
#include <cassert>
#include <iostream>

using namespace std;

cpu::impl::impl(){
	context = new ucontext_t();
	assert(!getcontext(context));
	running_thread = nullptr;
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
		swap_to_next_thread(true);
	}
	guard = 0;
	cpu::interrupt_enable();
}