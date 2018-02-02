#include "cpu_impl.h"
#include <cassert>

cpu::impl::impl(){
	context = new ucontext_t();
	assert(!getcontext(context));
	joined_context = nullptr;
	thread_to_join = nullptr;
}

cpu::impl::~impl(){
	delete context;
}

void cpu::impl::interrupt_ipi_handler(){
	//TODO: implement
}
void cpu::impl::interupt_timer_handler(){
	//TODO: implement
}