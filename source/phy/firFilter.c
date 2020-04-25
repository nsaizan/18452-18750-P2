/*
 * This code originates from the website:
 *      Shawn's DSP Tutorials
 */

#include <stdio.h>
#include <math.h>
#include "quadratureModulation.h"
#include "firFilter.h"

// array to hold input samples
float insamp_I[ BUFFER_LEN ];
float insamp_Q[ BUFFER_LEN ];
 
// FIR init for I data
void firFloatInit( void )
{
    memset( insamp_I, 0, sizeof( insamp ) );
}
 
// FIR init for Q data
void firFloatInit( void )
{
    memset( insamp_Q, 0, sizeof( insamp ) );
}
 
// the FIR filter function for I data
void firFloatA( double *coeffs, float *input, float *output,
       int length, int filterLength )
{
    float acc;     // accumulator for MACs
    double *coeffp; // pointer to coefficients
    float *inputp; // pointer to input samples
    int n;
    int k;
 
    // put the new samples at the high end of the buffer
    memcpy( &insamp_I[filterLength - 1], input,
            length * sizeof(float) );
 
    // apply the filter to each input sample
    for ( n = 0; n < length; n++ ) {
        // calculate output n
        coeffp = coeffs;
        inputp = &insamp_I[filterLength - 1 + n];
        acc = 0;
        for ( k = 0; k < filterLength; k++ ) {
            acc += (float)((*coeffp++) * (*inputp--));
        }
        output[n] = acc;
    }
    // shift input samples back in time for next time
    memmove( &insamp_I[0], &insamp_I[length],
            (filterLength - 1) * sizeof(float) );
 }

// the FIR filter function for Q data
void firFloatB( double *coeffs, float *input, float *output,
       int length, int filterLength )
{
    float acc;     // accumulator for MACs
    double *coeffp; // pointer to coefficients
    float *inputp; // pointer to input samples
    int n;
    int k;
 
    // put the new samples at the high end of the buffer
    memcpy( &insamp_Q[filterLength - 1], input,
            length * sizeof(float) );
 
    // apply the filter to each input sample
    for ( n = 0; n < length; n++ ) {
        // calculate output n
        coeffp = coeffs;
        inputp = &insamp_Q[filterLength - 1 + n];
        acc = 0;
        for ( k = 0; k < filterLength; k++ ) {
            acc += (float)((*coeffp++) * (*inputp--));
        }
        output[n] = acc;
    }
    // shift input samples back in time for next time
    memmove( &insamp_Q[0], &insamp_Q[length],
            (filterLength - 1) * sizeof(float) );
 
}

