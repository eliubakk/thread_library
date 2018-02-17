#include "cpu_impl.h"
#include "thread_globals.h"
#include <cassert>
#include <iostream>

using namespace std;

cpu::impl::impl(){
	context = new ucontext_t();
	assert(!getcontext(context));
	running_thread = nullptr;
	prev_thread = nullptr;
	finished = false;
}

cpu::impl::~impl(){
	delete context;
}

void cpu::impl::ipi_handler(){
	//TODO: Decide wtf to do here
}

void cpu::impl::timer_handler(){
	assert_interrupts_enabled();
	cpu::interrupt_disable();
	while(guard.exchange(1)){}
	//printf("swap from timer interrupt...");
	swap(false, true);
	guard = 0;
	assert_interrupts_disabled();
	cpu::interrupt_enable();
}