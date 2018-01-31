#include "thread_globals.h"

using namespace std;

queue<thread::impl*> thread_ready_queue;
queue<cpu::impl*> cpu_suspended_queue;