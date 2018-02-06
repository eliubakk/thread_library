#include "thread.h"
#include <iostream>

using namespace std;

mutex mutex1, mutex2;
cv cv1;
bool switch_b = false;

int counter = 0;
void thread_a(void *a){
	mutex1.lock();
	cout << "in thread a" << endl;
	while (switch_b == false) {
		cv1.wait(mutex1);
	}
	cout << "thread a finishing" << endl;
	mutex1.unlock();
}

void thread_b(void *a) {
	mutex2.lock();
	cout << "in thread b" << endl;
	while (switch_b == false) {
		cv1.wait(mutex2);
	}
	cout << "thread b finishing" << endl;
	mutex2.unlock();
}

void thread_c(void *a) {
	mutex1.lock();
	mutex2.lock();
	cout << "In thread c" << endl;
	switch_b = true;
	cv1.broadcast();
	cout << "thread c finishing" << endl;
	mutex1.unlock();
	mutex2.unlock();
}

void test_parent_thread(void *a){
    cout << "parent created!" << endl;
    thread t1((thread_startfunc_t)thread_a, a);
    thread t2((thread_startfunc_t)thread_b, a);
    thread t3((thread_startfunc_t)thread_c, a);
    cout << "parent is finishing" << endl;
}

int main(){
	int num = 100;
	cpu::boot(1, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 2);
	return 0;
}