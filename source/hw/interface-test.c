#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "firFilter.h"
#include "audioDeviceInterface.h"
#include "queue.h"
#include "quadratureModulation.h"
#include "interface.h"

#ifndef M_PI
#define M_PI (3.14159265)
#endif

#define DURATION (SAMPLE_RATE * 3)

int main(){
	/* open file pipe to phy and initialize everything */
	interface_init();

	/* setup some test data */
	int f = 2000;
	float sine[DURATION];
	for(unsigned long i = 0; i < DURATION; i++){
		sine[i] = sin(2* M_PI * f * i / SAMPLE_RATE);
	}
	float* audio_sample = sine;

	/* populate the send pipe */
	int test_rec_fd = open(rec_fifo, O_WRONLY);
	for(int sample = 0; sample < DURATION; sample++){
		printf("sending: %f\n", *(audio_sample + sample));
		char* bytes = (char *) audio_sample + sample;
		write(test_rec_fd, bytes, 4);
	}

	/* consume from send pipe and play audio */
	for(int sample = 0; sample < DURATION; sample++){
		file_interface_send();
	}

	/* close file pipe to phy and terminate portaudio */
	interface_deinit();
	return 0;
}
