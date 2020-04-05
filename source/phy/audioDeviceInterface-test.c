#include <stdio.h>
#include <math.h>
#include "audioDeviceInterface.h"
#include "queue.h"

#ifndef M_PI
#define M_PI (3.14159265)
#endif

int main(){
	float sine[24000];
	int f = 5000;
	for(unsigned long i = 0; i < 24000; i++){
		sine[i] = sin(2 * M_PI * f * i / 1000);
	}

	InitializeAudioInterface();
	for(unsigned long i = 0; i < 24000; i++){
		queue_push(&out_queue, sine[i]);
		//queue_pop(in_queue);
	}
	while(!queue_is_empty(&out_queue));
	TerminateAudioInterface();
	return 0;
}
