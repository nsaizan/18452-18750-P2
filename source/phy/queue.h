#ifndef FILE_QUEUE_SEEN
#define FILE_QUEUE_SEEN

/** @brief buffer data structure */
typedef struct {
    float buf[24000];
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
