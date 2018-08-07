#ifndef __S_THREAD_H__
#define __S_THREAD_H__

#include <pthread.h>

typedef void *(*SThreadWorker)(void *arg);

typedef void *(SThreadDestroyWorker)(void);

typedef struct _SThread SThread;

typedef enum _STreadState SThreadState;

struct _SThread {
    pthread_t thr;
    SThreadWorker worker;
    void *user_data;
    int is_running;
};

enum _SThreadState {
    READY = 0,
    RUNNING,
    TERMINATED
};

SThread *s_thread_create(SThreadWorker worker, void *data);

void s_thread_destory(SThread *thread);

int s_thread_run(SThread *thread);

void *s_thread_join(SThread *thread);

#endif /* __S_THREAD_H__ */
