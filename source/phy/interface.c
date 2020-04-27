#include <stdio.h>
#include <math.h>
#include <queue.h>
#include "audioDeviceInterface.h"
#include "quadratureModulation.h"
#include "firFilter.h"

#define DECIMATE_FACTOR (12)

uint32_t decimate_counter;

/* initializes both send/rec files and filter*/
void file_interface_init(void){
	file_rec_interface_init();
	file_send_interface_init();
	firFloatInitA();
	firFloatInitB();
	return;
}

/* initializes receive file */
void file_rec_interface_init(void){
        rec_fifo = "/tmp/rec_fifo";
        rec_fd = open(rec_fifo, O_WRONLY);
        return;
}

/* initializes send file */
void file_send_interface_init(void){
        send_fifo = "/tmp/send_fifo";
        send_fd = open(send_fifo, O_RDONLY);
        return;
}

/* de-inits both send/rec files */
void file_interface_deinit(void){
	file_rec_interface_deinit();
	file_send_interface_deinit();
}

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

/* sends data from file to audio device */
void file_interface_send(void){
	/* get a float from file */
	char file_data[sizeof(float)];
	read(send_fd, file_data, sizeof(file_data));
	//TODO deal with when it doesn't give enough data
	
	/* convert data to a float */
	float audio_out = (float) file_data;

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
        for(x = 0; x < SAMPLES; x++){
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
        for(x = 0; x < SAMPLES; x++){
                iq_pairs_filtered[x].i = i_arr_filtered[x];
                iq_pairs_filtered[x].q = q_arr_filtered[x];
        }

        	uint32_t arr_size = SAMPLES * 4;
        /* send iq_pairs to file */
	for(x = 0; x < SAMPLES; x++){
        	if(decimate_counter == 0){
			write(fifo, (char *) iq_pairs_filtered, 4);
		}
		decimate_counter++;
		decimate_counter %= DECIMATE_FACTOR;
        	iq_pairs_filtered+=4;
	}
        return;
}

