#include "thread_impl.h"
#include "thread_globals.h"
#include "cpu.h"
#include "cpu_impl.h"

thread::impl::impl(thread_startfunc_t func, void *arg) {
	context = new ucontext_t();
	getcontext(context);
	char *stack = new char [STACK_SIZE];
	context->uc_stack.ss_sp = stack;
	context->uc_stack.ss_size = STACK_SIZE;
	context->uc_stack.ss_flags = 0;
	context->uc_link = nullptr;

	makecontext(context, (void (*)())thread_wrapper, 2, func, arg);
}

thread::impl::~impl(){
	delete[] (char*)context->uc_stack.ss_sp;
	delete context;
}

void thread::impl::thread_wrapper(thread_startfunc_t func, void* arg){
	cpu::interrupt_enable();
	func(arg);
	cpu::interrupt_disable();
	cpu::self()->impl_ptr->finished = true;
	while (!cpu::self()->impl_ptr->running_thread->thread_join_queue.empty()) {
		thread_ready_queue_push(cpu::self()->impl_ptr->running_thread->thread_join_queue.front());
		cpu::self()->impl_ptr->running_thread->thread_join_queue.pop();
	}
	setcontext(cpu::self()->impl_ptr->context);
}