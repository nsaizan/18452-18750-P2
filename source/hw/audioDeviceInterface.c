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
	while(queue_is_full(&out_queue));
	queue_push(&out_queue, data);
	return;
}

float audio_receive(void){
	while(queue_is_empty(&in_queue));
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

	while(queue_is_full(&in_queue));
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
	printf("sample rate %d\n", SAMPLE_RATE);
	
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

	/* select output audio device details */
	outputParameters.device = Pa_GetDefaultOutputDevice();
	if(outputParameters.device == paNoDevice){
		fprintf(stderr,"Error: No default output device.\n"); 
		goto error;
	}
	outputParameters.channelCount = 1;
	outputParameters.sampleFormat = paFloat32;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;
	
	/* select input audio device details */
	inputParameters.device = Pa_GetDefaultInputDevice();
	if(inputParameters.device == paNoDevice){
		fprintf(stderr,"Error: No default input device.\n"); 
		goto error;
	}
	inputParameters.channelCount = 1;
	inputParameters.sampleFormat = paFloat32;
	inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;

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
