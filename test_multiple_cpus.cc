#include "thread.h"
#include <iostream>
#include <vector>
using namespace std;
vector<int>vec_num;
mutex mutex1;
mutex coutmut;
cv cv1;
void test_child_thread(void *a){
	mutex1.lock();
	int *arg = (int *)a;
	coutmut.lock();
	cout << "Child thread " << *arg << endl;
	coutmut.unlock();
	mutex1.unlock();
}
void test_parent_thread(void *a){
	coutmut.lock();
	cout << "Parent thread starts!" << endl;
	coutmut.unlock();
	for (int i = 0; i < 5; ++i){
		thread t1((thread_startfunc_t) test_child_thread, &vec_num[i]);
	}
	coutmut.lock();
	cout << "Parent thread ends!" << endl;
	coutmut.unlock();
}
int main(){
	for (int i = 0; i < 10; ++i){
		vec_num.push_back(i);
	}
	cpu::boot(3, (thread_startfunc_t)test_parent_thread ,(void *)10, 0, 1, 1);
	return 0;
}