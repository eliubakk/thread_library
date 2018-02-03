#include "thread.h"
#include <iostream>
using namespace std;
mutex note_lock;
bool note = false;
bool milk = false;

void test_bob_thread(void *a){
    note_lock.lock();
    if (!note){
        cout << "Bob left note" << endl;
        note = true;
        note_lock.unlock();
        if(!milk){
            milk = true;
            cout << "Bob bought milk" << endl;
        }
        note_lock.lock();
        note = false;
        cout << "Bob removed note" << endl;
        note_lock.unlock();    
    }
    else{
        note_lock.unlock();
    }
}
void test_alice_thread(void *a){
    note_lock.lock();
    if (!note){
        cout << "Alice left note" << endl;
        note = true;
        note_lock.unlock();
        if(!milk){
            milk = true;
            cout << "Alice bought milk" << endl;
        }
        note_lock.lock();
        note = false;
        cout << "Alice removed note" << endl;
        note_lock.unlock();    
    }
    else{
        note_lock.unlock();
    }
}
void test_parent_thread(void *a){
    int num = 10;
    thread t1((thread_startfunc_t)test_bob_thread, &num);
    thread t2((thread_startfunc_t)test_alice_thread, &num);
}
int main(){
    int num = 10;
    cpu::boot(1, (thread_startfunc_t) test_parent_thread, (void *) &num, false, false, 0);
    return 0;
}