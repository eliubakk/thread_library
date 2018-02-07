#include "thread.h"
#include <iostream>
using namespace std;

mutex mutex1;

void test_child_thread(void *a){
	mutex1.lock();
	mutex1.unlock();
}
void test_parent_thread(void *a){
	cout << "parent is called" << endl;
	for (int i = 0; i < 100000; ++i) {
		thread t1((thread_startfunc_t)test_child_thread, a);
		t1.join();
	}
	cout << "parent is finished" << endl;
}

int main(){
	int num = 100;
	cpu::boot(1, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 0);
	return 0;
}