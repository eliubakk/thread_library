#include "thread_impl.h"
#include "cpu.h"
#include "cpu_impl.h"

thread::impl::impl(thread_startfunc_t func, void *arg){
	context = new ucontext_t();
	getcontext(context);
	char *stack = new char [STACK_SIZE];
	context->uc_stack.ss_sp = stack;
	context->uc_stack.ss_size = STACK_SIZE;
	context->uc_stack.ss_flags = 0;
	context->uc_link = cpu::self()->impl_ptr->context;

	makecontext(context, (void (*)())func, 1, arg);
}

thread::impl::~impl(){
	delete[] (char*)context->uc_stack.ss_sp;
	delete context;
}