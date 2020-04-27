#include <stdio.h>
#include <math.h>
#include "queue.h"
#include "audioDeviceInterface.h"
#include "quadratureModulation.h"

#ifndef M_PI
#define M_PI (3.14159265)
#endif

float iq_modulate(float i, float q){
	float i_mod = i*iq_sine[iq_send_count];
	float q_mod = q*iq_cosine[iq_send_count];
	float sum = i_mod+q_mod;
	iq_send_count++;
	iq_send_count%=WAVE_TABLE_SIZE;
	return sum;
}

iq_pair iq_demodulate(float in){
	float i = in*iq_sine[iq_rec_count];
	float q = in*iq_cosine[iq_rec_count];
	iq_rec_count++;
	iq_rec_count%=WAVE_TABLE_SIZE;
	iq_pair vals = {i,q};
	return vals;
}

void iq_init(void){
	for(int x = 0; x < WAVE_TABLE_SIZE; x++){
		iq_sine[x] = sin(2 * M_PI * IQ_CARRIER_FREQ * x / SAMPLE_RATE);
		iq_cosine[x] = cos(2 * M_PI * IQ_CARRIER_FREQ * x / SAMPLE_RATE);
	}
	iq_send_count = 0;
	iq_rec_count = 0;
	return;
}

void iq_send(float i, float q){
	audio_send(iq_modulate(i,q));
	return;
}

iq_pair iq_receive(void){
	return iq_demodulate(audio_receive());
}
