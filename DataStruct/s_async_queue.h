#ifndef __S_ASYNC_QUEUE_H__
#define __S_ASYNC_QUEUE_H__

#include <pthread.h>

#include "s_macros.h"
#include "s_queue.h"

typedef struct _SAsyncQueue SAsyncQueue;

struct _SAsyncQueue {
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    SQueue queue;
    unsigned int waiting_threads;
    unsigned int ref_count;
}

SAsyncQueue *s_async_queue_create(void);

void s_async_queue_destroy(SAsyncQueue *queue);

void s_async_queue_push(SAyncQueue *queue, void *data);

void *s_async_queue_pop(SAyncQueue *queue);

#endif /* __S_AYNC_QUEUE_H__  */
