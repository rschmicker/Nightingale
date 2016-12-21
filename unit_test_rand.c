#include "unit_test_rand.h"

uint64_t pcg64_random_r(void* garbage){
	static int last_ix = 0;

	if(last_ix == 5) last_ix = 0;

	uint64_t arr[] = {
			2236597603191088455U,
			6674290611325526650U,
			7406426627614718268U,
			13506921166192047373U,
			13215999798518447804U,
			17200212620922802791U,
			3471701899310868630U,
			3048270304396094152U,
			8674324902522750117U,
			10852435343317765034U,
			18046083368183996421U
		};
	++last_ix;
	return arr[last_ix - 1];
}

void pcg64_srandom_r(void* garbage, char more_garbage, int last_garbage){ return; }
