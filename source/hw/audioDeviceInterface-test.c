#include <stdio.h>
#include <math.h>
#include "audioDeviceInterface.h"
#include "queue.h"

#ifndef M_PI
#define M_PI (3.14159265)
#endif

#define DURATION (SAMPLE_RATE * 3)

int main(){
	/* Test Sound Data Generation */
	float sine[DURATION];
	int f = 400;
	for(unsigned long i = 0; i < DURATION; i++){
		sine[i] = sin(2 * M_PI * f * i / SAMPLE_RATE);
	}

	/* Audio Interface Initialization */
	InitializeAudioInterface();

	/* Sending Output Data */
	for(unsigned long i = 0; i < DURATION; i++){
		audio_send(sine[i]);
		//audio_send(0);
	}
	audio_flush_out();

	/* Sending Recorded Data */
	for(unsigned long i = 0; i < DURATION; i++){
		audio_send(audio_receive());
	}
	audio_flush_out();

	/* Audio Interface De-Initialization */
	TerminateAudioInterface();
	return 0;
}
