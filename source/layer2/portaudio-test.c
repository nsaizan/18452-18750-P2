#include <stdio.h>
#include <math.h>
#include <portaudio.h>

#ifndef M_PI
#define M_PI (3.15159265)
#endif

#define TABLE_SIZE (200)
#define SAMPLE_RATE (44100)
#define FRAMES_PER_BUFFER (64)
#define NUM_SECONDS (3)

/* 
 * Data that is passed into the PortAudio Callback.
 * It can be used for processing input and output streams.
 */
typedef struct {
	float sine[TABLE_SIZE];
	int phase;
	char message[20];
} UserCallbackData;

/*
 * The function called by PortAudio when there is input data ready,
 * or when the output buffer is empty
 */
static int paStreamCallback(const void *inputBuffer, void *outputBuffer,
			    unsigned long framesPerBuffer,
			    const PaStreamCallbackTimeInfo* timeInfo,
			    PaStreamCallbackFlags statusFlags,
			    void *userData){
	UserCallbackData *data = (UserCallbackData *) userData;
	float *out = (float *) outputBuffer;
    	unsigned long i;

    	(void) timeInfo; /* Prevent unused variable warnings. */
    	(void) statusFlags;
    	(void) inputBuffer;
    
    	for(i=0; i<framesPerBuffer; i++){
        	*out++ = data->sine[data->phase];
        	data->phase += 1;
        	if( data->phase >= TABLE_SIZE ) data->phase -= TABLE_SIZE;
    	}
    
	return paContinue;
}

static void StreamFinished( void* userData )
{
	UserCallbackData *data = (UserCallbackData *) userData;
	printf( "Stream Completed: %s\n", data->message );
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



int main() {
	PaStream *stream; /* stream pointer received from Pa_OpenStream() */
	PaStreamParameters inputParameters; /* modify the #defines to select input device */
	PaStreamParameters outputParameters; /*modify the #defines to select output device */
	UserCallbackData userData; /* parameters that are passed into the streamCallback */
	
	/* initialise sinusoidal wavetable */
	for(int i=0; i<TABLE_SIZE; i++ ){
		userData.sine[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
	}
	userData.phase = 0;
	
	/* portaudio */
	PaError err;
	err = Pa_Initialize();
	if(err != paNoError){
		fprintf(stderr,"Error: Initalization failed.\n"); 
		goto error;
	}

	/* displays audio device details */
	displayAudioDevices();

	/* select audio device details */
	outputParameters.device = Pa_GetDefaultOutputDevice();
	if(outputParameters.device == paNoDevice){
		fprintf(stderr,"Error: No default output device.\n"); 
		goto error;
	}
	outputParameters.channelCount = 1;
	outputParameters.sampleFormat = paFloat32;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	/* open a stream */
	err = Pa_OpenStream(
			&stream, 
			NULL, 
			&outputParameters,
			SAMPLE_RATE, 
			FRAMES_PER_BUFFER, 
			paClipOff,
			paStreamCallback, 
			&userData);
	if(err != paNoError){
		fprintf(stderr,"Error: Failed to open stream.\n"); 
		goto error;
	}

	sprintf( userData.message, "No Message" );
	err = Pa_SetStreamFinishedCallback( stream, &StreamFinished );
	if( err != paNoError ) goto error;

	err = Pa_StartStream( stream );
	if( err != paNoError ) goto error;

	printf("Play for %d seconds.\n", NUM_SECONDS );
	Pa_Sleep( NUM_SECONDS * 1000 );

	err = Pa_StopStream( stream );
	if( err != paNoError ) goto error;

	err = Pa_CloseStream( stream );
    	if( err != paNoError ) goto error;
 	

	/* uninitializes portaudio */
	Pa_Terminate();
	printf("PortAudio Done\n");

	return 0;
error:
	Pa_Terminate();
    	fprintf( stderr, "An error occured while using the portaudio stream\n" );
	fprintf( stderr, "Error number: %d\n", err );
	fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
	return err;
}
