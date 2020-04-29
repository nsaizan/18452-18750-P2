#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "firFilter.h"
#include "audioDeviceInterface.h"
#include "queue.h"
#include "quadratureModulation.h"
#include "interface.h"

#ifndef M_PI
#define M_PI (3.14159265)
#endif

int main(){
	/* the fifo would be setup by phy */
	char* fifo = "/tmp/phy_tx_out";
	mkfifo(fifo, 0666);

	/* setup some test data */
	int f = 500;
	float sine[SAMPLE_RATE/f];
	for(unsigned long i = 0; i < SAMPLE_RATE/f; i++){
		sine[i] = sin(2* M_PI * f * i / SAMPLE_RATE);
	}

	/* open tx pipe */
	int fifo_fd = open(fifo, O_WRONLY);
	if(fifo_fd == -1){
		printf("Error: open failed\n");
		exit(0);
	}

	printf("Test will begin output audio samples...\n");

	/* constantly output data */
	unsigned long i = 0;
	unsigned long z = 0;
	while(1){	
		char* bytes = (char *) (sine + i);
		//printf("(%ld) sending: \t%d\t%d\t%d\t%d\t\t%f\n",
		//		z,bytes[0],bytes[1],bytes[3],bytes[4],sine[i]);
		write(fifo_fd, bytes, 4);
		i++;
		i%=SAMPLE_RATE/f;
		z++;
	}

	return 0;
}
