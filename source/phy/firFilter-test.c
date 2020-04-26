#include <stdio.h>
#include <math.h>
#include "firFilter.h"
#include "audioDeviceInterface.h"
#include "queue.h"
#include "quadratureModulation.h"

#ifndef M_PI
#define M_PI (3.14159265)
#endif

#define ITERATIONS (2)
#define DURATION (SAMPLES * ITERATIONS)

int main(){
	/* initialize iq pipeline */
	iq_init();

	/* initialize the filtering pipeline */
	firFloatInitA();
	firFloatInitB();

	/* display test info */
	printf("=========================================\n");
	printf("Testing IQ modulation and demodulation\n");
	printf("*This is WITH lpf to remove aliasing*\n");
	printf("For this test, I = -1, Q = 1\n");
	printf("=========================================\n");
	printf("input=(-1,1)\ti\t\tq\n");
	printf("-------\t\t------\t\t------\n");

	float post_mod[SAMPLES];
	iq_pair pair_post_demod[SAMPLES];
	float i_pre_filter[SAMPLES];
	float q_pre_filter[SAMPLES];
	float i_post_filter[SAMPLES];
	float q_post_filter[SAMPLES];

	for(unsigned long x = 0; x < ITERATIONS; x++){
		/* do the modulation */
		for(unsigned long y = 0; y < SAMPLES; y++){
			post_mod[y] = iq_modulate(-1, 1);
			pair_post_demod[y] = iq_demodulate(post_mod[y]);
			i_pre_filter[y] = pair_post_demod[y].i;
			q_pre_filter[y] = pair_post_demod[y].q;
		}
		/* filter the data */
		firFloatA( fir_coeffs, i_pre_filter, i_post_filter, SAMPLES, FILTER_LEN );
		firFloatB( fir_coeffs, q_pre_filter, q_post_filter, SAMPLES, FILTER_LEN );

		/* print the data */
		for(unsigned long y = 0; y < SAMPLES; y++){
			printf("%f\t%f\t%f\n", post_mod[y], 
					i_post_filter[y], q_post_filter[y]);
		}
	}

	return 0;
}
