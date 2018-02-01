#include "cpu_impl.h"
#include <cassert>

cpu::impl::impl(){
	context = new ucontext_t();
	assert(!getcontext(context));
	yielded = false;
}

cpu::impl::~impl(){
	delete context;
}