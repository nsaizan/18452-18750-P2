#include <stdio.h>
#include <math.h>
#include "audioDeviceInterface.h"
#include "queue.h"

#ifndef M_PI
#define M_PI (3.14159265)
#endif

int main(){
	/* Test Sound Data Generation */
	float sine[24000]; /* 24000 samples at 8000 sample rate is 3s */
	int f = 400; /* 400Hz frequency */
	for(unsigned long i = 0; i < 24000; i++){
		/* samples are at 1/8000 sec intervals */
		sine[i] = sin(2 * M_PI * f * i / 8000);
	}

	/* Audio Interface Initialization */
	InitializeAudioInterface();

	/* Sending Output Data */
	for(unsigned long i = 0; i < 24000; i++){
		queue_push(&out_queue, sine[i]);
		//queue_pop(in_queue);
	}
	while(!queue_is_empty(&out_queue));

	/* Audio Interface De-Initialization */
	TerminateAudioInterface();
	return 0;
}
