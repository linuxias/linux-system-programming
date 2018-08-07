#ifndef __S_QUEUE_H__
#define __S_QUEUE_H__

#include "s_list.h"

typedef struct _SQueue SQueue;

struct _SQueue {
    SList *head;
    SList *tail;
    int length;
};

SQueue *s_queue_create(void);

void s_queue_destroy(SQueue *queue);

void s_queue_push(SQueue *queue, void *data);

void *s_queue_pop(SQueue *queue);

SList *s_queue_peek_tail_link(SQueue *queue);

SList *s_queue_peek_head_link(SQueue *queue);

#endif /* __S_QUEUE_H__ */
