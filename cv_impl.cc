#include "cv_impl.h"
#include "thread_globals.h"

using namespace std;

cv::impl::impl(){
	//TODO?
}

cv::impl::~impl(){
	//TODO
}

bool cv::impl::wake_up(){
	assert_interrupts_disabled();
	if(waiting_queue.empty())
		return false;
	thread_ready_queue_push(waiting_queue.front(), true);
    waiting_queue.pop();
   	return true;
}