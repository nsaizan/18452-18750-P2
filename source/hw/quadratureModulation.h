#ifndef FILE_IQ_SEEN
#define FILE_IQ_SEEN

#include <stdio.h>
#include <math.h>
#include "queue.h"
#include "audioDeviceInterface.h"

#ifndef M_PI
#define M_PI (3.15159265)
#endif

#define IQ_CARRIER_FREQ (6000)
#define WAVE_TABLE_SIZE (SAMPLE_RATE/IQ_CARRIER_FREQ)

float iq_sine[WAVE_TABLE_SIZE];
float iq_cosine[WAVE_TABLE_SIZE];
int iq_send_count;
int iq_rec_count;

typedef struct {
	float i;
	float q;
} iq_pair;

/* takes in a i/q pair, outputs a quadrature modulated value */
float iq_modulate(float i, float q);

/* takes in a quadrature modulate value, outputs an i/q pair */
iq_pair iq_demodulate(float in);

/* initializes the wave tables and state counters */
void iq_init(void);

/* sends an i/q pair into the audio out pipeline */
void iq_send(float i, float q);

/* reads an i/q pair from the audio in pipeline */
iq_pair iq_receive(void);

#endif
