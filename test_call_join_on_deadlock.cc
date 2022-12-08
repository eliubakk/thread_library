#include "thread.h"
#include <iostream>

using namespace std;

mutex mut1, mut2;

void test_child_1_thread(void *a){
	cout << "child 1 locking mut1" << endl;
	mut1.lock();
	cout << "child 1 owns mut1" << endl;
	thread::yield();
	cout << "child 1 locking mut2" << endl;
	mut2.lock();
	cout << "child 1 owns mut2" << endl;
	mut2.unlock();
	cout << "child 1 released mut2" << endl;
	mut1.unlock();
	cout << "child 1 released mut1" << endl;
}

void test_child_2_thread(void *a) {
	cout << "child 2 locking mut2" << endl;
	mut2.lock();
	cout << "child 2 owns mut2" << endl;
	cout << "child 2 locking mut1" << endl;
	mut1.lock();
	cout << "child 2 owns mut1" << endl;
	mut1.unlock();
	cout << "child 2 released mut1" << endl;
	mut2.unlock();
	cout << "child 2 released mut2" << endl;
}

void test_parent_thread(void *a){
	int *arg = (int *)a;
	cout << "parent starts: " << *arg << endl;
	thread t1((thread_startfunc_t)test_child_1_thread, a);
	thread t2((thread_startfunc_t)test_child_2_thread, a);
	cout << "about to call join on child 1" << endl;
	t1.join();
	cout << "called join on child 1" << endl;
	cout << "parent is done" << endl;

}

int main(){
	int num = 100;
	cpu::boot(1, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 0);
	return 0;
}