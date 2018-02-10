#include "thread.h"
#include <iostream>
using namespace std;

mutex mutex1;
cv cv1;

void test_child_thread(void *a){
	mutex1.unlock();
	catch (const runtime_error& error) {
		cout << "Thread tried to unlock mutex it does not currently own" << endl;
	}
	cv1.wait(mutex1);
	catch (const runtime_error& error) {
		cout << "Thread tried to wait on mutex it does not currently own" << endl;
	}
}
void test_parent_thread(void *a){
	cout << "parent is called" << endl;
	for (int i = 0; i < 10; ++i) {
		thread t1((thread_startfunc_t)test_child_thread, a);
	}
	cout << "parent is finished" << endl;
}

int main(){
	int num = 100;
	cpu::boot(1, (thread_startfunc_t)test_parent_thread ,(void *)&num, 0, 0, 0);
	return 0;
}