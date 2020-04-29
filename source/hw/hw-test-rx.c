#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include "firFilter.h"
//#include "audioDeviceInterface.h"
//#include "queue.h"
//#include "quadratureModulation.h"
//#include "interface.h"

#ifndef M_PI
#define M_PI (3.14159265)
#endif

//#define SAMPLE_RATE = (48000)

#ifndef DURATION
#define DURATION (SAMPLE_RATE * 3)
#endif

int main(){
	/* open file pipe to phy and initialize everything */
	char* fifo = "/tmp/phy_rx_in";
	mkfifo(fifo, 0666);

	/* open rx pipe */
	printf("Test will now open the rx pipe\n");
	printf("(the other end of the rx pipe must be opened to continue)\n");
	fflush(stdout);
	int fifo_fd = open(fifo, O_RDONLY);
	if(fifo_fd == -1){
		printf("Error: open failed\n");
		exit(0);
	}

	printf("Successfully opened the rx pipe!\n");
	printf("Test will begin printing received data...\n");
	/* constantly read in data and print it */
	unsigned long count = 0;
	while(1){
		char data[sizeof(float)];
		int cnt = 0;
		while(cnt < 8){
			cnt += read(fifo_fd, &data[cnt], 1);
		}
		float val1 = ((float *) data)[0];
		float val2 = ((float *) data)[1];
		printf("(%ld) received: \t%f\t\t%f\n", count, val1, val2);
		count++;

	}

	return 0;
}
