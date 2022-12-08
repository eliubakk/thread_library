#include "thread.h"
#include <iostream>
using namespace std;
void test_child_thread(void *a){
	cout << "child is done!" << endl;
}
void test_parent_thread(void *a){
	int *arg = (int *)a;
	cout << *arg << endl;
	thread t1((thread_startfunc_t)test_child_thread, a);
	cout << "parent works!" << endl;
	thread::yield();
	cout << "parent continues!" << endl;
}

int main(){
	int num = 100;
	cpu::boot(1, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 0);
	return 0;
}