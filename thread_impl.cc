#include "thread_impl.h"

thread::impl::impl(){
	getcontext(context);
}