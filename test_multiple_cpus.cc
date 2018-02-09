#include "thread.h"
#include <iostream>
#include <vector>
using namespace std;
vector<int>vec_num;
mutex mutex1;
cv cv1;
void test_child_thread(void *a){
	mutex1.lock();
	int *arg = (int *)a;
	cout << "Child thread " << *arg << endl;
	mutex1.unlock();
}
void test_parent_thread(void *a){
	cout << "Parent thread starts!" << endl;
	for (int i = 0; i < 5; ++i){
		thread t1((thread_startfunc_t) test_child_thread, &vec_num[i]);
	}
	cout << "Parent thread ends!" << endl;
}
int main(){
	for (int i = 0; i < 10; ++i){
		vec_num.push_back(i);
	}
	cpu::boot(3, (thread_startfunc_t)test_parent_thread ,(void *)10, 1, 0, 1);
	return 0;
}