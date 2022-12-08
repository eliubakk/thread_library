#include "thread.h"
#include <iostream>
using namespace std;
mutex mutex1, mutex2;
cv cv1;
void test_child_second_thread(void *a){
	mutex2.lock();
	cout << "Child second thread starts!" << endl;
	thread::yield();
	cout << "Child second thread ends!" << endl;
	mutex2.unlock();
	mutex2.unlock();
	cv1.wait(mutex2);
}
void test_child_thread(void *a){
	mutex1.lock();
	cout << "Child first thread starts!" << endl;
	thread t2((thread_startfunc_t)test_child_second_thread, a);
	mutex1.unlock();
	mutex1.unlock();
	mutex1.lock();
	cout << "Child first thread ends!" << endl;
	thread::yield();
	mutex1.unlock();
	mutex1.lock();
	cv1.signal();
	cv1.broadcast();
	mutex1.lock();
}
void test_parent_thread(void *a){
	thread t1((thread_startfunc_t)test_child_thread, a);
	t1.join();
}
int main(){
	int num = 100;
	cpu::boot(1, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 0);
	return 0;
}