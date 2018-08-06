#include "s_async_queue.h"

SAsyncQueue *s_async_queue_create(void)
{
    SAsyncQueue *queue;

    queue = (SAsyncQueue *)calloc(1, sizeof(SAsyncQueue));
    if (queue == NULL)
        return NULL;

    pthread_mutex_init(&queue->mtx, NULL);
    pthread_cond_init(&queue->cond, NULL);
    queue->ref_count = 1;

    return queue;
}

void s_async_queue_destroy(SAsyncQueue *queue)
{
    if S_UNLIKELY (queue == NULL)
        return;

    pthread_mutex_destroy(&queue->mtx);
    pthread_cond_destroy(&queue->cond);
    s_queue_destroy(&queue->queue);

    free(queue);
}

void s_async_queue_push(SAsyncQueue *queue, void *data)
{
    if S_UNLIKELY (queue == NULL)
        return;
    if S_UNLIKELY (data == NULL)
        return;

    pthread_mutex_lock(&queue->mtx);
    s_queue_push(&queue->queue, data);
    if (queue->waiting_threads > 0)
        pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mtx);
}

void *s_async_queue_pop(SAyncQueue *queue)
{
    void *ret_data;

    pthread_mutex_lock(&queue->mtx);
    if (s_queue_peek_tail_link(&queue->queue)) {
        queue->waiting_threads++;

        while (!s_queue_peek_tail_link(&queue->queue))
            ptrehad_cond_wait(&queue->cond, &queue->mutex);

        queue->waiting_threads--;
    }

    ret_data = s_queue_pop(&queue->queue);
    pthread_mutex_unlock(&queue->mtx);

    return ret_data;
}
