#include <stdio.h>
#include <stdlib.h>

#include "s_list.h"
#include "s_macros.h"
#include "s_queue.h"

SQueue *s_queue_create(void)
{
    SQueue *que = NULL;
    que = (SQueue *)calloc(1, sizeof(SQueue));

    return que;
}

void s_queue_destroy(SQueue *queue)
{
    if S_UNLIKELY (queue == NULL)
        return;

    if S_UNLIKELY (queue->head == NULL)
        return;
    s_list_free(queue->head);

    free(queue);
    queue = NULL;
}

void s_queue_push(SQueue *queue, void *data)
{
    if S_UNLIKELY (queue == NULL)
        return;

    queue->tail = s_list_append(queue->tail, data);
    if (queue->tail->next)
        queue->tail = queue->tail->next;
    else
        queue->head = queue->tail;
    queue->length++;
}

void *s_queue_pop(SQueue *queue)
{
    SList *node;
    void * data;

    if S_UNLIKELY (queue == NULL)
        return NULL;

    if (queue->head) {
        node = queue->head;
        data = node->data;
        queue->head = node->next;

        if (queue->head)
            queue->head->pre = NULL;
        else
            queue->tail = NULL;
        s_list_free(node);
        
        queue->length--;

        return data;
    }

    return NULL;
}

SList *s_queue_peek_tail_link(SQueue *queue)
{
    if S_UNLIKELY (queue == NULL)
        return NULL;

    return queue->tail;
}

SList *s_queue_peek_head_link(SQueue *queue)
{
    if S_UNLIKELY (queue == NULL)
        return NULL;

    return queue->head;
}
