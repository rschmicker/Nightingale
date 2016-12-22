#include "unit_test_rand.h"

uint64_t pcg64_random_r(void* garbage){
	static int last_ix = 0;

	static bool e_pass1_finished = false;
	static bool d_pass1_finished = false;

	static bool e_pass2_finished = false;
	static bool d_pass2_finished = false;

	static bool e_pass3_finished = false;
	static bool d_pass3_finished = false;

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

	if(!e_pass1_finished){
		if(last_ix == 5){
			last_ix = 0;
			e_pass1_finished = true;
		} 		
		++last_ix;
		return arr[last_ix - 1];
	}
	else if(!d_pass1_finished){
		if(last_ix == 5){
			last_ix = 0;
			d_pass1_finished = true;
		} 		
		++last_ix;
		return arr[last_ix - 1];
	}
	else if(!e_pass2_finished){
		if(last_ix == 7){
			last_ix = 0;
			e_pass2_finished = true;
		} 		
		++last_ix;
		return arr[last_ix - 1];
	}
	else if(!d_pass2_finished){
		if(last_ix == 7){
			last_ix = 0;
			d_pass2_finished = true;
		} 		
		++last_ix;
		return arr[last_ix - 1];
	}
	else if(!e_pass3_finished){
		if(last_ix == 11){
			last_ix = 0;
			e_pass3_finished = true;
		} 		
		++last_ix;
		return arr[last_ix - 1];
	}
	else if(!d_pass3_finished){
		if(last_ix == 11){
			last_ix = 0;
			d_pass3_finished = true;
		} 		
		++last_ix;
		return arr[last_ix - 1];
	}

	
}

void pcg64_srandom_r(void* garbage, char more_garbage, int last_garbage){ return; }
