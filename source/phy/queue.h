#ifndef FILE_QUEUE_SEEN
#define FILE_QUEUE_SEEN

#ifndef SAMPLE_RATE
#define SAMPLE_RATE (44100)
#endif

#define MAX_LEN (SAMPLE_RATE * 3)

/** @brief buffer data structure */
typedef struct {
    float buf[MAX_LEN];
    int front;
    int back;
    int maxlen;
} queue_t;

/** @brief uart send & recieve buffers */
queue_t volatile out_queue;
queue_t volatile in_queue;

/** @brief function headers, functions described in uart.c */
int queue_push(queue_t volatile *queue, float data);

float queue_pop(queue_t volatile *queue);

int queue_is_empty(queue_t volatile *queue);

int queue_is_full(queue_t volatile *queue);

int queue_get_len(queue_t volatile *queue);

float queue_peak_next_byte(queue_t volatile *queue);

void queue_init(queue_t volatile *queue);

#endif
