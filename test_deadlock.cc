#include "thread.h"
#include <iostream>

using namespace std;

mutex mut1, mut2;

void test_child_thread(void *a){
	cout << "child starts" << endl;
	cout << "child locking mut2" << endl;
	mut2.lock();
	cout << "child owns mut2" << endl;
	cout << "child locking mut1" << endl;
	mut1.lock();
	cout << "child owns mut1" << endl;
	mut1.unlock();
	cout << "child released mut1" << endl;
	mut2.unlock();
	cout << "child released mut2" << endl;
	cout << "child is done" << endl;
}

void test_parent_thread(void *a){
	int *arg = (int *)a;
	cout << "parent starts: " << *arg << endl;
	thread t1((thread_startfunc_t)test_child_thread, a);
	cout << "parent locking mut1" << endl;
	mut1.lock();
	cout << "parent owns mut1" << endl;
	thread::yield();
	cout << "parent locking mut2" << endl;
	mut2.lock();
	cout << "parent owns mut2" << endl;
	mut2.unlock();
	cout << "parent released mut2" << endl;
	mut1.unlock();
	cout << "parent released mut1" << endl;
	cout << "parent is done" << endl;

}

int main(){
	int num = 100;
	cpu::boot(1, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 0);
	return 0;
}