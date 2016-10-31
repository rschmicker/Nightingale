#include "pcg_variants.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){

pcg32_random_t rng, rng2;
int some_constant = 100000;
uint64_t seed = 642364365436;

pcg32_srandom_r(&rng, time(NULL), (intptr_t)&some_constant);
pcg32_srandom_r(&rng2, seed, some_constant);

uint32_t rand = pcg32_random_r(&rng);
printf("Time Test:\t%u\n", rand);

uint32_t rand_seed = pcg32_random_r(&rng2);
printf("Seed Test:\t%u\n", rand_seed);

return 0;
}
