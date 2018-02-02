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