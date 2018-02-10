#include "thread.h"
#include <iostream>
#include <vector>
using namespace std;
mutex mutex1;
cv cv1, cv2;
bool done = false;
vector<bool>vec_bool;
vector<int>vec_num;
int counter = 0;
void test_child_thread(void *a){
    mutex1.lock();
    int *arg = (int *)a;
    if (!vec_bool[*arg]){
        ++counter;
        cv2.signal();
        cv1.wait(mutex1);
    }
    cout << "child " << *arg << " is done!" << endl;
    mutex1.unlock();
}
void test_parent_thread(void *a){
    mutex1.lock();
    cout << "parent created!" << endl;
    for (int i = 0; i < 10; ++i){
        thread t1((thread_startfunc_t)test_child_thread, &vec_num[i]);
    }
    while (counter != 0){
        cv2.wait(mutex1);
    }
    for (int i = 0; i < 10; ++i){
        vec_bool[i] = true;
    }
    cv2.broadcast();
    mutex1.unlock();
}
int main(){
    int num = 100;
    for (int i = 0; i < 10; ++i){
        vec_bool.push_back(false);
        vec_num.push_back(i);
    }
    cpu::boot(5, (thread_startfunc_t) test_parent_thread, (void *) &num, true, false, 0);
    return 0;
}