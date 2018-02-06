#include "thread.h"
#include <iostream>
#include <vector>
using namespace std;
vector<cv *>vec_cv;
vector<int>vec_int;
vector<bool>vec_bool;
mutex child;
cv child_done;
int counter = 0;
void test_child_signal_thread(void *a){
	for (int i = 4; i >= 0; --i){
		vec_bool[i] = true;
		vec_cv[i]->signal();
	}
}

void test_child_thread(void *a){
	child.lock();
	int* arg = (int*)a;
	cout << "Child thread " << *arg << " starts!" << endl;
	while(!vec_bool[*arg]){
		vec_cv[*arg]->wait(child);
	}
	cout << "child thread " << *arg << " finished!" << endl; 
	++counter;
	if (counter == 5){
		child_done.signal();
	}
	child_done.broadcast();
	child.unlock();
}
void test_child_print_thread(void* a){
	child.lock();
	int* arg = (int*)a;
	cout << "child thread printing " << *arg << endl;
	child.unlock();
}
void test_parent_thread(void *a){
	for (int i = 0; i < 5; ++i){
		thread t1((thread_startfunc_t)test_child_thread, &vec_int[i]);
	}
	int num = 10;
	thread t1((thread_startfunc_t)test_child_signal_thread, &num);
	for (int i = 0; i < 5; ++i){
		thread t1((thread_startfunc_t)test_child_print_thread, &vec_int[i]);
		t1.join();
	}
}
int main(){
	int num = 100;
	for (int i = 0; i < 5; ++i){
		vec_cv.push_back(new cv());
		vec_bool.push_back(false);
		vec_int.push_back(i);
	}
	cpu::boot(1, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 0);
	return 0;
}