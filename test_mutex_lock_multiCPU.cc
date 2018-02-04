#include "thread.h"
#include <iostream>

mutex mutex1;

using namespace std;

void test_child_thread(void *a){
	mutex1.lock();
	cout << "child is done!" << endl;
	mutex1.unlock();
}
void test_parent_thread(void *a){
	int *arg = (int *)a;
	cout << *arg << endl;
	for (int i = 0; i < 5; ++i){
		thread t1((thread_startfunc_t)test_child_thread, a);
	}
	cout << "parent is done" << endl;
}
int main(){
	int num = 100;
	cpu::boot(5, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 2);
	return 0;
}