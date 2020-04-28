#ifndef FILE_FIR_SEEN
#define FILE_FIR_SEEN

/*
 * This code originates from the website:
 * 	Shawn's DSP Tutorials
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

/* Maximum number of inputs that can be handled in one function call */
#define MAX_INPUT_LEN 80

/* Maximum length of filter than can be handled */
#define MAX_FLT_LEN 63

/* Buffer to hold all of the input samples */
#define BUFFER_LEN      (MAX_FLT_LEN - 1 + MAX_INPUT_LEN)

/* Number of new audio samples per filter call 
 * Must be less than MAX_INPUT_LEN */
#define SAMPLES  1

/* Length of the fir filter 
 * Must be less than MAX_FLT_LEN */
#define FILTER_LEN  63

/* coefficients for the fir filter with 2.5kHz passband edge freq */
static double fir_coeffs[ FILTER_LEN ] =
{
	-0.000004,  0.000160,  0.000350,  0.000600,  0.000799,  0.000799,  0.000452,  -0.000322,  -0.001452,  -0.002670,  -0.003531,  -0.003512,  -0.002197,  0.000513,  0.004226,  0.008014,  0.010552,  0.010457,  0.006752,  -0.000646,  -0.010605,  -0.020735,  -0.027742,  -0.028115,  -0.019043,  0.000708,  0.030177,  0.066092,  0.103323,  0.135856,  0.158061,  0.165944,  0.158061,  0.135856,  0.103323,  0.066092,  0.030177,  0.000708,  -0.019043,  -0.028115,  -0.027742,  -0.020735,  -0.010605,  -0.000646,  0.006752,  0.010457,  0.010552,  0.008014,  0.004226,  0.000513,  -0.002197,  -0.003512,  -0.003531,  -0.002670,  -0.001452,  -0.000322,  0.000452,  0.000799,  0.000799,  0.000600,  0.000350,  0.000160,  -0.000004
};

void firFloatInitA( void );

void firFloatInitB( void );

void firFloatA( double *coeffs, float *input, float *output,
		int length, int filterLength );

void firFloatB( double *coeffs, float *input, float *output,
		int length, int filterLength );

#endif
