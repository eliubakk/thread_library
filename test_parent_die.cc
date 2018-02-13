#include "thread.h"
#include <iostream>
using namespace std;
mutex mutex1;
cv cv1;
bool done = false;
void test_child_thread(void *a){
	mutex1.lock();
	cout << "Child thread starts!" << endl;
	while(!done){
		cv1.wait(mutex1);
	}
	cout << "Child thread ends!" << endl;
	mutex1.unlock();
}
void test_parent_thread(void *a){
	cout << "Parent thread starts!" << endl;
	for (int i = 0; i < 1; ++i){
		thread t1((thread_startfunc_t)test_child_thread, a);
	}
	cout << "Parent thread ends!" << endl;
	done = true;
	cv1.signal();
}

int main(){
	int num = 100;
	cpu::boot(1, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 0);
	return 0;
}