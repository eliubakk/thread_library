#include "thread.h"
#include <iostream>
using namespace std;
mutex mutex1;
void test_child_thread(void *a){
	mutex1.lock();
	cout << "Child thread starts!" << endl;
	mutex1.unlock();
}
void test_parent_thread(void *a){
	mutex1.lock();
	cout << "Parent thread starts!" << endl;
	int num = 100;
	thread t1((thread_startfunc_t) test_child_thread, &num);
}
int main(){
	int num = 100;
	cpu::boot(1, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 0);
	return 0;
}