#include <stdio.h>
#include <math.h>
#include "audioDeviceInterface.h"
#include "queue.h"
#include "quadratureModulation.h"

#ifndef M_PI
#define M_PI (3.14159265)
#endif

#define DURATION (SAMPLE_RATE * 3)

#define RAW_OUTPUT  1

int main(){
	/* initialize iq pipeline */
	iq_init();

	/* display test info */
	if(!RAW_OUTPUT){
		printf("=========================================\n");
		printf("Testing IQ modulation and demodulation\n");
		printf("*This is without lpf to remove aliasing*\n");
		printf("For this test, I = -1, Q = 1\n");
		printf("=========================================\n");
		printf("input=(-1,1)\ti\t\tq\n");
		printf("-------\t\t------\t\t------\n");
	}
	for(unsigned long i = 0; i < DURATION/(30*48); i++){
		float out = iq_modulate(-1, 1);
		iq_pair pair = iq_demodulate(out);
		if(!RAW_OUTPUT){
			printf("%f\t%f\t%f\n", out, pair.i, pair.q);
		} else {
			printf("%f,%f,%f\n", out, pair.i, pair.q);
		}
	}

	return 0;
}
