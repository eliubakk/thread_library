#include "thread.h"
#include <cassert>

using namespace std;

thread::thread(thread_startfunc_t, void *){
	assert(false);
} // create a new thread
thread::~thread(){
	assert(false);
}

void thread::join(){
	assert(false);
}                        // wait for this thread to finish

void thread::yield(){
	assert(false);
}                // yield the CPU

/*
 * Move constructor and move assignment operator.  Implementing these is
 * optional in Project 2.
 
thread(thread&&);
thread& operator=(thread&&);*/