#ifndef _CV_IMPL_H
#define _CV_IMPL_H

#include "cv.h"
#include "thread_impl.h"
#include <queue>

class cv::impl{
	public:
		impl();
		~impl();
		
		bool wake_up();
		std::queue<thread::impl*> waiting_queue;
};

#endif