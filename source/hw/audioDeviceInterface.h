#ifndef FILE_AUDIO_SEEN
#define FILE_AUDIO_SEEN

#include <stdio.h>
#include <math.h>
#include <portaudio.h>
#include "queue.h"

#ifndef M_PI
#define M_PI (3.15159265)
#endif

#define FRAMES_PER_BUFFER (1)
#define SAMPLE_SILENCE (0)

/* User Data Interface Structure*/
typedef struct {
	queue_t in;
	queue_t out;
} UserCallbackData;

/* User Interface to Stream Callback Function */
UserCallbackData userData;

/* PortAudio Stream Instance */
PaStream *stream_in;
PaStream *stream_out;

/* Device Setup Properties */
PaStreamParameters inputParameters;
PaStreamParameters outputParameters;

void audio_flush_out(void);

void audio_send(float data);

float audio_receive(void);

/* Function that is called when audio out buffer is empty */
static int paOutputStreamCallback(const void *inputBuffer, void *outputBuffer,
			    unsigned long framesPerBuffer,
			    const PaStreamCallbackTimeInfo* timeInfo,
			    PaStreamCallbackFlags statusFlags,
			    void *userData);

/* Function that is called when audio in buffer is full */
static int paInputStreamCallback(const void *inputBuffer, void *outputBuffer,
			    unsigned long framesPerBuffer,
			    const PaStreamCallbackTimeInfo* timeInfo,
			    PaStreamCallbackFlags statusFlags,
			    void *userData);

/* Displays Information when the Stream is Closed */
static void StreamFinished(void* userData);

/* List Visible Audio Device Details */
void displayAudioDevices();

/* Sets up Audio Stream */
int InitializeAudioInterface(void);

/* Shuts down Audio Stream */
int TerminateAudioInterface(void);

#endif
