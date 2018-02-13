#include "thread.h"
#include <iostream>
#include <vector>
thread* thread_ptr;
using namespace std;
mutex mutex1;
cv cv1;
bool done = false;
vector<thread*>vec_thread;
void test_child_thread(void *a){
	mutex1.lock();
	cout << "Child thread starts!" << endl;
	vec_thread[0]->join();
	cout << "Child thread ends!" << endl;
	mutex1.unlock();
}
void test_parent_thread(void *a){
	cout << "Parent thread starts!" << endl;
	vec_thread.push_back(new thread((thread_startfunc_t)test_child_thread, a));
	cout << "Parent thread ends!" << endl;
}

int main(){
	int num = 100;
	cpu::boot(1, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 0);
	return 0;
}