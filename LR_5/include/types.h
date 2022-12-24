#include "stdlib.h"
#include "stdio.h"

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
