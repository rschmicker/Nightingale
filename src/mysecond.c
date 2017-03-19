#include "mysecond.h"

double mysecond()
{
    struct timeval tp;
    int i;

    i = gettimeofday(&tp, (void *)NULL);
    return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}
