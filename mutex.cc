#include "mutex.h"
#include "mutex_impl.h"
#include "cpu.h"
#include "cpu_impl.h"
#include "thread_globals.h"
#include <cassert>
#include <stdexcept>

using namespace std;

mutex::mutex(){
    impl_ptr = new impl();
}

mutex::~mutex(){
    delete impl_ptr;
}

void mutex::lock(){
    cpu::interrupt_disable();
    while(guard.exchange(1)){}
    try{
        impl_ptr->lock();
    }catch(bad_alloc& e){
        guard = 0;
        cpu::interrupt_enable();
        throw e;
    }
    guard = 0;
    cpu::interrupt_enable();
}

void mutex::unlock(){
    cpu::interrupt_disable();
    while(guard.exchange(1)){}
    try{
        impl_ptr->unlock();
    }
    catch(bad_alloc& e){
        guard = 0;
        cpu::interrupt_enable();
        throw e;
    }
    catch(runtime_error& e){
        guard = 0;
        cpu::interrupt_enable();
        throw e;
    }
    guard = 0;
    cpu::interrupt_enable();
}


//  * Move constructor and move assignment operator.  Implementing these is
//  * optional in Project 2.
 
// mutex(mutex&&);
// mutex& operator=(mutex&&);