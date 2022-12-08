#include "thread.h"
#include <iostream>
using namespace std;
mutex mutex1, mutex2;
void test_child_thread(void *a){
	mutex1.lock();
	cout << "child is done!" << endl;
	mutex2.unlock();
	mutex1.unlock();
}
void test_parent_thread(void *a){
	int *arg = (int *)a;
	cout << *arg << endl;
	for (int i = 0; i < 1; ++i){
		thread t1((thread_startfunc_t)test_child_thread, a);
	}
}
int main(){
	int num = 100;
	cpu::boot(1, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 0);
	return 0;
}