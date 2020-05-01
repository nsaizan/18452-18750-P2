#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "firFilter.h"
#include "audioDeviceInterface.h"
#include "queue.h"
#include "quadratureModulation.h"

#ifndef M_PI
#define M_PI (3.14159265)
#endif

#define ITERATIONS (100)
#define DURATION (SAMPLES * ITERATIONS)

#define RAW_OUTPUT 1
#define NOISE_AMP 0

int main(){
	/* Test Sound Data Generation */
        int f1 = 2500;
        float sine1[SAMPLE_RATE/f1*10];
        for(unsigned long i = 0; i < SAMPLE_RATE/f1*10; i++){
                sine1[i] = sin(2 * M_PI * f1 * i / SAMPLE_RATE);
        }
        int f2 = 1000;
        float cosine2[SAMPLE_RATE/f2*10];
        for(unsigned long i = 0; i < SAMPLE_RATE/f2*10; i++){
                cosine2[i] = cos(2 * M_PI * f2 * i / SAMPLE_RATE);
        }

	/* initialize iq pipeline */
	iq_init();

	/* initialize the filtering pipeline */
	firFloatInitA();
	firFloatInitB();

	/* display test info */
	if(!RAW_OUTPUT){
		printf("SAMPLES = %d, ITERATIONS = %d\n",SAMPLES,ITERATIONS);
		printf("=========================================\n");
		printf("Testing IQ modulation and demodulation\n");
		printf("*This is WITH lpf to remove aliasing*\n");
		printf("For this test, I = -1 + 1kHz, Q = 2.5kHz\n");
		printf("=========================================\n");
		printf("i_ini\t\tq_ini\t\tinput\t\ti_pre\t\tq_pre\t\ti_post\t\tq_post\n");
		printf("------\t\t------\t\t-------\t\t------\t\t------\t\t------\t\t------\n");
	} else {
		printf("data = [");
	}
	float i_pre_mod[SAMPLES];
	float q_pre_mod[SAMPLES];
	float post_mod[SAMPLES];
	float with_noise[SAMPLES];
	iq_pair pair_post_demod[SAMPLES];
	float i_pre_filter[SAMPLES];
	float q_pre_filter[SAMPLES];
	float i_post_filter[SAMPLES];
	float q_post_filter[SAMPLES];

	int index1 = 0;
	int index2 = 0;
	for(unsigned long x = 0; x < ITERATIONS; x++){
		/* do the modulation */
		for(unsigned long y = 0; y < SAMPLES; y++){
			i_pre_mod[y] = -1 + cosine2[index2];
			q_pre_mod[y] = sine1[index1];
			post_mod[y] = iq_modulate(i_pre_mod[y], q_pre_mod[y]);
			float noise = 2*NOISE_AMP*(((float)(rand() % 10000)/10000)-0.5);
			with_noise[y] = post_mod[y] + noise;
			pair_post_demod[y] = iq_demodulate(with_noise[y]);
			i_pre_filter[y] = pair_post_demod[y].i;
			q_pre_filter[y] = pair_post_demod[y].q;

			index1++;
			index2++;
			index1 %= (SAMPLE_RATE/f1*10);
			index2 %= (SAMPLE_RATE/f2*10);
		}
		/* filter the data */
		firFloatA( fir_coeffs, i_pre_filter, i_post_filter, SAMPLES, FILTER_LEN );
		firFloatB( fir_coeffs, q_pre_filter, q_post_filter, SAMPLES, FILTER_LEN );

		/* print the data */
		for(unsigned long y = 0; y < SAMPLES; y++){
			if(!RAW_OUTPUT){
				printf("%f\t%f\t",
					i_pre_mod[y], q_pre_mod[y]);
				printf("%f\t%f\t%f\t%f\t%f\n", with_noise[y], 
					i_pre_filter[y], q_pre_filter[y],
					i_post_filter[y], q_post_filter[y]);
			} else {
				if(y == (SAMPLES - 1) && x == (ITERATIONS - 1)){
					printf("%f,%f,",
						i_pre_mod[y], q_pre_mod[y]);
					printf("%f,%f,%f,%f,%f]\n", with_noise[y], 
						i_pre_filter[y], q_pre_filter[y],
						i_post_filter[y], q_post_filter[y]);
				} else {
					printf("%f,%f,",
						i_pre_mod[y], q_pre_mod[y]);
					printf("%f,%f,%f,%f,%f,", with_noise[y], 
						i_pre_filter[y], q_pre_filter[y],
						i_post_filter[y], q_post_filter[y]);
				}
			}
		}
	}

	return 0;
}
