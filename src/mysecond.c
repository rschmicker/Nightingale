#include "mysecond.h"

double mysecond()
{
    struct timeval tp;
    gettimeofday(&tp, 0);
    return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}
