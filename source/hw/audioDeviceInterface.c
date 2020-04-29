#include <stdio.h>
#include <math.h>
#include <portaudio.h>
#include "audioDeviceInterface.h"
#include "queue.h"

void audio_flush_out(void){
	while(!queue_is_empty(&out_queue));
	return;
}

void audio_send(float data){
	while(queue_is_full(&out_queue)){
		//printf("transmit q full!\n");	
	}
	queue_push(&out_queue, data);
	return;
}

float audio_receive(void){
	while(queue_is_empty(&in_queue)){
		//printf("receive q empty!\n");	
	}
	return queue_pop(&in_queue);
}

/*
 * The function called by PortAudio when the audio output buffer is empty.
 */
static int paOutputStreamCallback(const void *inputBuffer, void *outputBuffer,
			    unsigned long framesPerBuffer,
			    const PaStreamCallbackTimeInfo* timeInfo,
			    PaStreamCallbackFlags statusFlags,
			    void *userData){
	UserCallbackData *data = (UserCallbackData *) userData;
	float *out = (float *) outputBuffer;

    	(void) timeInfo; /* Prevent unused variable warnings. */
    	(void) statusFlags;
    	(void) inputBuffer;

	if(queue_is_empty(&out_queue)){
		*out = SAMPLE_SILENCE;
	} else {
        	*out = queue_pop(&out_queue);
	}
	//printf("%fi\n",*out);
    
	return paContinue;
}

/*
 * The function called by PortAudio when there is audio input data ready,
 */
static int paInputStreamCallback(const void *inputBuffer, void *outputBuffer,
			    unsigned long framesPerBuffer,
			    const PaStreamCallbackTimeInfo* timeInfo,
			    PaStreamCallbackFlags statusFlags,
			    void *userData){
	UserCallbackData *data = (UserCallbackData *) userData;
	float *in = (float *) inputBuffer;

    	(void) timeInfo; /* Prevent unused variable warnings. */
    	(void) statusFlags;
    	(void) outputBuffer;

	//printf("receiving\n");
	while(queue_is_full(&in_queue)){
		printf("receive q is full!\n");		
	}
        queue_push(&in_queue, *in);
    
	return paContinue;
}

static void StreamFinished( void* userData )
{
	printf( "Stream Completed\n");
}

void displayAudioDevices(){
	/* Get Number of Devices */
	PaHostApiIndex device_count = Pa_GetDeviceCount();
	printf("\nNumber of Audio Devices: %d\n",(int) device_count);

	/* List Details of Devices */
	PaDeviceInfo di;
	PaDeviceIndex pi = Pa_GetDefaultInputDevice();
	PaDeviceIndex po = Pa_GetDefaultOutputDevice();
	for(int i = 0; i < device_count; i++){
		di = *Pa_GetDeviceInfo(i);
		if(i == pi && i == po){
			printf("\n---Device %d---   <--Host Preferred Input/Output Device\n",i);
		}else if(i == pi){
			printf("\n---Device %d---   <--Host Preferred Input Device\n",i);
		}else if(i == po){
			printf("\n---Device %d---   <--Host Preferred Output Device\n",i);
		}else{
			printf("\n---Device %d---\n",i);
		}
		printf("name: %s\n",di.name);
		printf("input channels: %d\n",di.maxInputChannels);
		printf("output channels: %d\n",di.maxOutputChannels);
		printf("sample rate: %f\n",di.defaultSampleRate);
	}
	printf("\n");
	
	return;
}

int InitializeAudioInterface(void){
	/* setup buffers between portaudio and higher level software */
	queue_init(&out_queue);
	queue_init(&in_queue);
		
	/* portaudio initialization*/
	PaError err;
	err = Pa_Initialize();
	if(err != paNoError){
		fprintf(stderr,"Error: Initalization failed.\n"); 
		goto error;
	}

	/* displays audio device details */
	/* if you want to manually select an audio device use this */
	displayAudioDevices();

	/* allow the user to manually select audio devices */
	int input_device_id = -2;
	int output_device_id = -2;
	int num_devices = Pa_GetDeviceCount();
	if(num_devices < 0){
		printf("Error: Pa_CountDevices returned 0x%x\n", num_devices);
		goto error;
	}
	while(input_device_id == -2 || output_device_id == -2){
		printf("Please give the device number for your desired audio devices\n");
		printf("(If you are unsure '-1' will select the system default)\n");
		printf("Input Device:");
		scanf("%d",&input_device_id);
		printf("\n");
		printf("Output Device:");
		scanf("%d",&output_device_id);
		printf("\n");
		if(input_device_id == -1){
			input_device_id = Pa_GetDefaultInputDevice();
			if(input_device_id == paNoDevice){
				printf("Error: No default input device\n");
				input_device_id = -2;
			}
		} else if(input_device_id > num_devices || input_device_id < -1){
			printf("Error: Input Device ID is out of range\n");
			input_device_id = -2;
		}
		if(output_device_id == -1){
			output_device_id = Pa_GetDefaultOutputDevice();
			if(output_device_id == paNoDevice){
				printf("Error: No default output device\n");
				output_device_id = -2;
			}
		} else if(output_device_id > num_devices || output_device_id < -1){
			printf("Error: Output Device ID is out of range\n");
			output_device_id = -2;
		}
		
	}

	
	/* select output audio device details */
	outputParameters.device = output_device_id;
	outputParameters.channelCount = 1;
	outputParameters.sampleFormat = paFloat32;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;
	
	/* select input audio device details */
	inputParameters.device = input_device_id;
	inputParameters.channelCount = 1;
	inputParameters.sampleFormat = paFloat32;
	inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;

	/* confirm that the SAMPLE_RATE is valid for the chosen devices */
	err = Pa_IsFormatSupported(&inputParameters, &outputParameters, SAMPLE_RATE);
	if(err == paFormatIsSupported){
		printf("Device setup parameters accepted\n");
	} else {
		printf("Error: Device setup parameters are invalid!\n");
	}	

	/* open output stream */
	err = Pa_OpenStream(
			&stream_out, 
			NULL, 
			&outputParameters,
			SAMPLE_RATE, 
			FRAMES_PER_BUFFER, 
			paClipOff,
			paOutputStreamCallback, 
			&userData);
	if(err != paNoError){
		fprintf(stderr,"Error: Failed to open output stream.\n"); 
		goto error;
	}

	/* open input stream */
	err = Pa_OpenStream(
			&stream_in,  
			&inputParameters,
			NULL,
			SAMPLE_RATE, 
			FRAMES_PER_BUFFER, 
			paClipOff,
			paInputStreamCallback, 
			&userData);
	if(err != paNoError){
		fprintf(stderr,"Error: Failed to open input stream.\n"); 
		goto error;
	}

	err = Pa_SetStreamFinishedCallback( stream_out, &StreamFinished );
	if( err != paNoError ) goto error;
	err = Pa_SetStreamFinishedCallback( stream_in, &StreamFinished );
	if( err != paNoError ) goto error;

	err = Pa_StartStream( stream_out );
	if( err != paNoError ) goto error;
	err = Pa_StartStream( stream_in );
	if( err != paNoError ) goto error;


	printf("PortAudio Streams Started\n");
	return 0;
error:
	Pa_Terminate();
    	fprintf( stderr, "An error occured while using the portaudio streams\n" );
	fprintf( stderr, "Error number: %d\n", err );
	fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
	return err;
}

int TerminateAudioInterface(void){
	PaError err;
	err = Pa_StopStream( stream_out );
	if( err != paNoError ) goto error;
	err = Pa_StopStream( stream_in );
	if( err != paNoError ) goto error;

	err = Pa_CloseStream( stream_out );
    	if( err != paNoError ) goto error;
	err = Pa_CloseStream( stream_in );
    	if( err != paNoError ) goto error;
 	

	/* uninitializes portaudio */
	Pa_Terminate();
	printf("PortAudio Streams Terminated\n");

	return 0;
error:
	Pa_Terminate();
    	fprintf( stderr, "An error occured while closing the portaudio streams\n" );
	fprintf( stderr, "Error number: %d\n", err );
	fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
	return err;
}
