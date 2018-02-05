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
	if(waiting_queue.empty())
		return false;
	thread::impl* t = waiting_queue.front();
    waiting_queue.pop();
   	thread_ready_queue_push(t);
   	return true;
}