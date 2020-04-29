#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "queue.h"
#include "audioDeviceInterface.h"
#include "quadratureModulation.h"
#include "firFilter.h"
#include "interface.h"

#define DECIMATE_FACTOR (12)

unsigned int decimate_counter;

/*********************************************
 *		INITIALIZATION
 *********************************************/

/* initializes receive file */
void file_rec_interface_init(void){
        rec_fifo = "/tmp/phy_rx_in";
        rec_fd = open(rec_fifo, O_WRONLY);
	if(rec_fd == -1){
                printf("ERROR: failed to open inbound fifo\n");
                exit(0);
        }
        return;
}

/* initializes send file */
void file_send_interface_init(void){
        send_fifo = "/tmp/phy_tx_out";
        send_fd = open(send_fifo, O_RDONLY);
	if(send_fd == -1){
                printf("ERROR: failed to open outbound fifo\n");
                exit(0);
        }
        return;
}

/* initializes everything in hw layer */
void interface_init(void){
	/* initialize the receive file pipe */
	file_rec_interface_init();

	/* initialize the sending file pipe */
	file_send_interface_init();
	
	/* initialize the FIR buffer for I */
	firFloatInitA();

	/* initialize the FIr buffer for Q */
	firFloatInitB();

	/* initialize the quadrature modulation data */
	iq_init();

	/* initialize the PortAudio library and queues */
	InitializeAudioInterface();
	return;
}

/*********************************************
 *		DE-INITIALIZATION
 *********************************************/

/* de-initializes receive file */
void file_rec_interface_deinit(void){
        close(rec_fd);
        return;
}

/* de-initializes send file */
void file_send_interface_deinit(void){
        close(send_fd);
        return;
}

/* de-inits both send/rec files */
void file_interface_deinit(void){
	file_rec_interface_deinit();
	file_send_interface_deinit();
}

/* de-initializes everything in hw layer */
void interface_deinit(void){
	/* empty the audio output buffer */
	audio_flush_out();

	/* file interface */
	file_interface_deinit();

	/* portaudio interface */
	TerminateAudioInterface();

	return;
}

/*********************************************
 *		DATA FLOW CALLS
 *********************************************/
unsigned long z = 0;
/* sends data from file to audio device */
void file_interface_send(void){
	/* get a float from file */
	char file_data[sizeof(float)];
	int cnt = 0;
	while(cnt < 4){
		cnt += read(send_fd, &file_data[cnt], 1);
	}
	//printf("(%ld) receive: \t%d\t%d\t%d\t%d",z,file_data[0],file_data[1],file_data[2],file_data[3]);
	z++;

	/* convert data to a float */
	float audio_out = ((float *) file_data)[0];
	//printf("\t\t%f\n", audio_out);

	/* send to audio interface */
	audio_send(audio_out);

	return;
}

/* receives data from audio device to send to file */
void file_interface_receive(void){
        /* get demodulated iq pairs */
        iq_pair iq_pairs[SAMPLES];
        float i_arr[SAMPLES];
        float q_arr[SAMPLES];
        for(int x = 0; x < SAMPLES; x++){
		//printf("consuming from receive q\n");
                iq_pairs[x] = iq_receive();
                i_arr[x] = iq_pairs[x].i;
                q_arr[x] = iq_pairs[x].q;
        }

        /* low pass filter iq_pairs */
        float i_arr_filtered[SAMPLES];
        float q_arr_filtered[SAMPLES];
        firFloatA( fir_coeffs, i_arr, i_arr_filtered, SAMPLES, FILTER_LEN );
        firFloatB( fir_coeffs, q_arr, q_arr_filtered, SAMPLES, FILTER_LEN );

	/* recombine i & q arrays into single iq array */
        iq_pair iq_pairs_filtered[SAMPLES];
        for(int x = 0; x < SAMPLES; x++){
                iq_pairs_filtered[x].i = i_arr_filtered[x];
                iq_pairs_filtered[x].q = q_arr_filtered[x];
        }

        /* send iq_pairs to file */
	for(int x = 0; x < SAMPLES; x++){
        	if(decimate_counter == 0){
			write(rec_fd, (char *) &(iq_pairs_filtered[x]), 8);
		}
		decimate_counter++;
		decimate_counter %= DECIMATE_FACTOR;
	}
        return;
}

