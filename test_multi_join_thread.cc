#include "thread.h"
#include <iostream>
using namespace std;

mutex mut1;
cv cv1;
bool thread_b_finish = false;
int thread_a_counter = 0;

void test_thread_b(void *arg){
	cout << "thread b start!" << endl;
	mut1.lock();
	while(!thread_b_finish){
		cout << "thread b waiting..." << endl;
		cv1.wait(mut1);
	}
	cout << "thread b continues" << endl;
	cout << "thread b done!" << endl;
	mut1.unlock();
}

void test_thread_a(void *arg){
	thread* b = (thread*)arg;
	mut1.lock();
	int id = thread_a_counter;
	thread_a_counter++;
	cout << "thread a " << id << " start!" << endl;
	cout << "thread a " << id << " signals thread b" << endl;
	cout << "thread a " << id << " join thread b" << endl;
	cv1.signal();
	mut1.unlock();
	b->join();
	mut1.lock();
	cout << "thread a " << id << " joined thread b" << endl;
	cout << "thread a " << id << " done!" << endl;
	mut1.unlock();
}

void test_parent_thread(void *num){
	mut1.lock();
	cout << "parent creates thread b" << endl;
	thread b((thread_startfunc_t)test_thread_b, num);
	cout << "parent creates 5 thread a" << endl;
	for(int i = 0; i < 5; ++i){
		thread t1((thread_startfunc_t)test_thread_a, &b);
	}
	cout << "parent yields" << endl;
	mut1.unlock();
	thread::yield();
	mut1.lock();
	cout << "parent lets thread b finish" << endl;
	thread_b_finish = true;
	cv1.signal();
	cout << "parent yields" << endl;
	mut1.unlock();
	thread::yield();
	mut1.lock();
	cout << "parent creates another thread a" << endl;
	thread a((thread_startfunc_t)test_thread_a, &b);
	cout << "parent join thread a" << endl;
	mut1.unlock();
	a.join();
	cout << "parent joined thread a" << endl;	
	cout << "parent done!" << endl;
}

int main(){
	int num = 100;
	cpu::boot(1, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 0);
	return 0;
}