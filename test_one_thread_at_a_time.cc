#include "thread.h"
#include <iostream>
using namespace std;

mutex mutex1;
int switch1 = 0;


void test_child_thread(void *a){
	mutex1.lock();
	cout << "a child thread has been called" << endl;
	cout << "child is done!" << endl;
	switch1 = 1;
	mutex1.unlock();
}
void test_parent_thread(void *a){
	int *arg = (int *)a;
	cout << *arg << endl;
	cout << "parent is called" << endl;
	for (int i = 0; i < 200; ++i) {
		switch1 = 0;
		thread t1((thread_startfunc_t)test_child_thread, a);
		while (switch1 == 0) {}
	}
	cout << "parent is finished" << endl;
}

int main(){
	int num = 100;
	cpu::boot(1, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 0);
	return 0;
}