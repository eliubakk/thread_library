#include "thread.h"
#include <iostream>
#include <stdexcept>
using namespace std;

mutex mutex1;
cv cv1;

void test_child_thread(void *a){
	mutex1.lock();
	try{
		cv1.wait(mutex1);
	}
	catch(bad_alloc& e){
	}
	mutex1.unlock();
}
void test_parent_thread(void *a){
	cout << "parent is called" << endl;
	try{
		for (int i = 0; i < 100000; ++i) {
			thread t1((thread_startfunc_t)test_child_thread, a);
		}
	}catch(bad_alloc& e){
		
	}
	cout << "parent is finished" << endl;
}

int main(){
	int num = 100;
	cpu::boot(1, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 0);
	return 0;
}