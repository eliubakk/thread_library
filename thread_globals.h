#ifndef _THREAD_GLOBALS_H
#define _THREAD_GLOBALS_H

#include <queue>
#include "thread.h"
#include "cpu.h"
#include <ucontext.h>

extern std::queue<thread::impl*> thread_ready_queue;
extern std::queue<cpu*> cpu_suspended_queue;

void thread_ready_queue_push(thread::impl* t, bool have_guard = 0);

void swap(bool from_cpu, bool push_to_ready_queue);
	
#endif