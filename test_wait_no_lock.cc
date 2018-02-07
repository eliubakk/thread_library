#include "thread.h"
#include <iostream>
using namespace std;
mutex mutex1;
cv cv1;
void test_child_thread(void *a){
	mutex1.lock();
	cout << "Child thread starts!" << endl;
	mutex1.unlock();
	cv1.wait(mutex1);
	cout << "Child thread ends!" << endl;
}
void test_parent_thread(void *a){
	cv1.signal();
	cv1.broadcast();
	cout << "Parent thread starts!" << endl;
	thread t1((thread_startfunc_t)test_child_thread, a);
	cv1.signal();
	cv1.broadcast();
	t1.join();
	cout << "Parent thread ends!" << endl;
}
int main(){
	int num = 100;
	cpu::boot(1, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 0);
	return 0;
}