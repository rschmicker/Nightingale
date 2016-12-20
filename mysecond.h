/* A gettimeofday routine to give access to the wall
   clock timer on most UNIX-like systems.

   This version defines two entry points -- with 
   and without appended underscores, so it *should*
   automagically link with FORTRAN */

#include <stdlib.h>
#include <sys/time.h>

double mysecond();
