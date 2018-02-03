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

/*
    Questionable style. A user program may not be written in the style we recommend for concurrent programs, 
    but it may still lie within the bounds of how a program is allowed to use the thread library without causing an exception.
    Here are a few examples; none of these should be considered an error for Project 2.
        Calling thread::yield.
        Signaling without holding the lock (some may not even consider this poor style).
        Deadlock. Even trivial deadlocks are legal, such as a thread trying to acquire a mutex it has already locked, or a thread trying to join with itself.
        Calling cv::wait without checking a condition in a while loop.
        Calling cv::wait on one cv with different mutexes.
        A thread that exits while holding a mutex (the mutex stays locked). 
    Misuse of thread functions A user program may use a thread function in a manner that is explicitly disallowed by Mesa monitors.
    In particular, a thread is not allowed to release a mutex it hasn't locked. Your thread library should detect such misuses and throw a std::runtime_error exception.
    Resource exhaustion. The process may exhaust a resource needed by the OS. In this project, the main resource used by the OS is memory.
    If the thread library is unable to service a request due to lack of memory, it should throw a std::bad_alloc exception.
    Undefined behavior Some programming errors cause undefined behavior in C++, e.g., using uninitialized variables or pointers to free memory.
    Examples in this project include creating a thread with a bad function pointer (e.g., nullptr) and destroying a thread object while it is still in use by thread::join.
    Your thread library may behave arbitrarily for such errors. 

All programs with well-defined behavior (i.e., all but the last category in the list above) can and should be used when testing your thread library,
and your thread library should produce well-defined results for them.

The other source of errors are bugs in the OS code itself (in this case, your thread library).
While developing the OS, the best behavior in this case is for the OS to detect the bug quickly and assert (this is called a panic in kernel parlance).
These error checks are essential in debugging concurrent programs, because they help flag error conditions early.
Use assertion statements copiously in your thread library to check for bugs in your code (for reference, 1/8 of the lines of code in my thread library are assertions).

To make it easier for you to check for errors related to interrupts, the infrastructure provides two functions, 
assert_interrupts_disabled and assert_interrupts_enabled that your thread library can call to check that the status of interrupts is as you expect. */