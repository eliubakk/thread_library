#include "mutex_impl.h"

using namespace std;

mutex::impl::impl(){
	owner = nullptr;
	status = UNLOCKED;
}

mutex::impl::~impl(){
	//TODO
}