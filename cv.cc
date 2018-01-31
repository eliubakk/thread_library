#include "cv.h"
#include <cassert>

using namespace std;

cv::cv(){
    assert(false);
}

cv::~cv(){
    assert(false);
}

void cv::wait(mutex&){
    assert(false);
}                // wait on this condition variable

void cv::signal(){
    assert(false);
}                      // wake up one thread on this condition
                                        // variable
void cv::broadcast(){
    assert(false);
}                   // wake up all threads on this condition
                                        // variable

//  * Move constructor and move assignment operator.  Implementing these is
//  * optional in Project 2.
 
cv::cv(cv&&){
    assert(false);
}

cv& cv::operator=(cv&&){
    assert(false);
}