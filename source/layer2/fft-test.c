#include <stdio.h>
#include <math.h>
#include "kiss_fftr.h"

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

/*
 * Notes on the FFT Functionality:
 *
 * The frequency range of the fft output will be equal
 * to the number of samples in the input data.
 *
 * Example:
 * 	
 * 	If nfft is 1000 (cx_in has 1000 samples),
 * 	...
 * 	then cx_out will have a bucket for each integer
 * 	frequency from 0Hz to 1000Hz.
 *
 * 	and
 *
 * 	If nfft is 2000 (cx_in has 2000 samples),
 * 	...
 * 	then cx_out will have a bucket for each integer
 * 	frequency from 0Hz to 2000Hz.
 */
int main() {
	printf("\nThis is a test to verify that kiss-fft is working\n");
	
	/* nfft must be even */
	int nfft = 1000;	/*# of time samples*/

	/* input array */
	kiss_fft_scalar data_in[nfft];
	double f1 = 3; /*Hz*/
	double f2 = 50; /*Hz*/
	for(int i = 0; i < nfft; i++){
		data_in[i] = sin(2*M_PI*f1*i/1000) * sin(2*M_PI*f2*i/1000);
	}

	/* output array */
	kiss_fft_cpx data_out[nfft/2+1];

	/* setup fft config & input/output buffers */
	kiss_fftr_cfg cfg = kiss_fftr_alloc(nfft, 0, 0, 0);
	kiss_fft_scalar *cx_in = data_in;
	kiss_fft_cpx *cx_out = data_out;

	/*
	for(int i = 0; i < nfft; i++){
		cx_in[i] = data_in[i];
	}
	*/

	printf("\ninput:\n");
	for(int i = 0; i < nfft; i++){
		printf("[%d]\tr:%f\ti:%f\n", i, cx_in[i], 0.0);
	}

	kiss_fftr(cfg, cx_in, cx_out);

	printf("\noutput:\n");
	for(int i = 0; i < nfft/2+1; i++){
		printf("[%d]\tr:%f\ti:%f\n", i, cx_out[i].r, cx_out[i].i);
	}
	printf("\n");

	free(cfg);

	return 0;

}
