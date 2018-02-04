#include "thread.h"
#include <iostream>
using namespace std;
void test_child_thread(void *a){
	cout << "child start!" << endl;
	thread::yield();
	cout << "child is done!" << endl;
}
void test_parent_thread(void *a){
	int *arg = (int *)a;
	cout << *arg << endl;
	thread t1((thread_startfunc_t)test_child_thread, a);
	t1.join();
	cout << "parent works!" << endl;
	
	cout << "parent continues!" << endl;
}

int main(){
	int num = 100;
	cpu::boot(5, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 0);
	return 0;
}