#include "cpu_impl.h"

cpu::impl::impl(){
	getcontext(context);
}