#ifndef _THREAD_GLOBALS_H
#define _THREAD_GLOBALS_H

#include <queue>
#include "thread.h"
#include "cpu.h"

extern std::queue<thread::impl*> thread_ready_queue;
extern std::queue<cpu*> cpu_suspended_queue;


#endif