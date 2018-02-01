#include "thread_globals.h"

using namespace std;

queue<thread::impl*> thread_ready_queue;
queue<cpu*> cpu_suspended_queue;