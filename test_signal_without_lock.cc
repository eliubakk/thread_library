#include "thread.h"
#include <iostream>

using namespace std;

mutex mutex1;
cv cv1;
int counter = 0;

int counter = 0;
void test_child_thread(void *a){
	++counter;
	cv1.signal();
    mutex1.lock();
    int *arg = (int *)a;
    cout << "child " << *arg << " is done!" << endl;
    mutex1.unlock();
}
void test_parent_thread(void *a){
    mutex1.lock();
    cout << "parent created!" << endl;
    thread t1((thread_startfunc_t)test_child_thread, a);
    while (counter != 0){
        cv1.wait(mutex1);
    }
    mutex1.unlock();
}

int main(){
	int num = 100;
	cpu::boot(1, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 2);
	return 0;
}