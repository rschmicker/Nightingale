#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Random number call for unit test
//-----------------------------------------------------------------------------
uint64_t pcg64_random_r(void* garbage);

//-----------------------------------------------------------------------------
// fake seeding for unit test
//-----------------------------------------------------------------------------
void pcg64_srandom_r(void* garbage, char more_garbage, int last_garbage);

#define pcg64_random_t char
#define pcg128_t char