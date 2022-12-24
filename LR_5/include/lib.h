#ifndef __LIB_H__
#define __LIB_H_

#include "stdlib.h"
#include "stdio.h"
#include "types.h"

/*
#if OS == APPLE
typedef short os_int;
typedef long double os_float;
//#endif

#elif OS == UNIX
typedef long os_int;
typedef double os_float;
//#endif

#elif OS == WIN32
typedef int os_int;
typedef float os_float;
#endif
*/

extern "C" os_float Square(os_float A, os_float B);
extern "C" os_int *Sort(os_int *array, os_int size);

#endif
