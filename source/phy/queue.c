#include "queue.h"

/**
 * functions for manipulating audio queues
 *
 * these queues provide a buffer between software and the audio device library
 */

/**
 * @brief queue_push
 *
 * @param queue - the queue being pushed to
 * @param data - the data being pushed onto the queue
 *
 * @return 0 if the push was successful, -1 otherwise
 */
int queue_push(queue_t volatile *queue, float data){
    /* get the next data slot in the queue */
    int new_back = queue->back + 1;
    
    /* handle the circularity of the queue */
    if (new_back >= queue->maxlen) new_back = 0;

    /* return if queue is full */
    if (new_back == queue->front) return -1;

    /* push data onto queue */
    queue->buf[queue->back] = data;

    /* increment queue's back address */
    queue->back = new_back;

    return 0;
}

/**
 * @brief queue_pop
 *
 * @param queue - the queue being popped from
 *
 * @return the data that has been popped
 */
float queue_pop(queue_t volatile *queue){
    /* get value at front of queue */
    float value = queue->buf[queue->front];

    /* move the queue front address */
    queue->front = queue->front + 1;

    /* correct for overflows */
    if (queue->front >= queue->maxlen) queue->front = 0;

    return value;
}

/**
 * @brief queue_is_empty
 *
 * @param queue - the queue to check for emptiness
 *
 * @return 1 if queue is empty, 0 otherwise
 */
int queue_is_empty(queue_t volatile *queue){
    if (queue->front == queue->back) return 1;

    return 0;
}

/**
 * @brief queue_is_full
 *
 * @param queue - the queue to check for fullness
 *
 * @return 1 if queue is full, 0 otherwise
 */
int queue_is_full(queue_t volatile *queue){
    if (queue->back == (queue->front - 1) % queue->maxlen) return 1;

    return 0;
}

/**
 * @brief queue_init
 *
 * @param queue - the queue to initialize
 *
 * @return none
 */
void queue_init(queue_t volatile *queue){
    queue->front = 0;
    queue->back = 0;
    queue->maxlen = MAX_LEN;

    return;
}
