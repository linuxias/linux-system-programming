#include <stdio.h>
#include <stdlib.h>

#include "s_thread.h"
#include "s_macros.h"

SThread *s_thread_create(SThreadWorker worker, void *data)
{
    SThread *thread;

    thread = (SThread *)calloc(1, sizeof(struct _SThread));
    if S_UNLIKELY (thread == NULL) {
        return NULL;
    }

    thread->worker = worker;
    thread->user_data = data;

    return thread;
}

void s_thread_destory(SThread *thread)
{
    if (thread) {
        free(thread);
        thread = NULL;
    }
}

int s_thread_run(SThread *thread)
{
    int ret;

    if S_UNLIKELY (thread == NULL)
        return -1;

    ret = pthread_create(&thread->thr, NULL, thread->worker, thread->user_data);
    if (ret != 0)
        return -1;

    thread->is_running = RUNNING;

    return 0;
}

void *s_thread_join(SThread *thread)
{
    void *ret = NULL;
    if S_UNLIKELY (thread == NULL)
        return NULL;

    pthread_join(thread->thr, &ret);

    return ret;
}
