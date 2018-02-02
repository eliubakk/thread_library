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
	cout << "in ipi_handler" << endl;
}

void cpu::impl::timer_handler(){
	cpu::interrupt_disable();
	swapcontext(cpu::self()->impl_ptr->running_thread->context,
				cpu::self()->impl_ptr->context);
	cpu::interrupt_enable();
}