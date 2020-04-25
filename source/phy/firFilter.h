/*
 * This code originates from the website:
 * 	Shawn's DSP Tutorials
 */

#include <stdio.h>
#include <math.h>
#include "quadratureModulation.h"

// maximum number of inputs that can be handled
// in one function call
#define MAX_INPUT_LEN   80
// maximum length of filter than can be handled
#define MAX_FLT_LEN     63
// buffer to hold all of the input samples
#define BUFFER_LEN      (MAX_FLT_LEN - 1 + MAX_INPUT_LEN)

#define SAMPLES 	80

#define FILTER_LEN  63
double coeffs[ FILTER_LEN ] =
{
  -0.0448093,  0.0322875,   0.0181163,   0.0087615,   0.0056797,
   0.0086685,  0.0148049,   0.0187190,   0.0151019,   0.0027594,
  -0.0132676, -0.0232561,  -0.0187804,   0.0006382,   0.0250536,
   0.0387214,  0.0299817,   0.0002609,  -0.0345546,  -0.0525282,
  -0.0395620,  0.0000246,   0.0440998,   0.0651867,   0.0479110,
   0.0000135, -0.0508558,  -0.0736313,  -0.0529380,  -0.0000709,
   0.0540186,  0.0766746,   0.0540186,  -0.0000709,  -0.0529380,
  -0.0736313, -0.0508558,   0.0000135,   0.0479110,   0.0651867,
   0.0440998,  0.0000246,  -0.0395620,  -0.0525282,  -0.0345546,
   0.0002609,  0.0299817,   0.0387214,   0.0250536,   0.0006382,
  -0.0187804, -0.0232561,  -0.0132676,   0.0027594,   0.0151019,
   0.0187190,  0.0148049,   0.0086685,   0.0056797,   0.0087615,
   0.0181163,  0.0322875,  -0.0448093
};//TODO update values to 10Khz cutoff

void firFloatAInit( void );

void firFloatBInit( void );

void firFloatA( double *coeffs, double *input, double *output,
		int length, int filterLength );

void firFloatB( double *coeffs, double *input, double *output,
		int length, int filterLength );
