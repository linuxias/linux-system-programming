#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "daemon.h"
#include "server_type.h"

#define PORT_NUMBER 6200
#define THREAD_MAX 22

static pthread_t thr[THREAD_MAX];
static pthread_cond_t gcond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t gmtx = PTHREAD_MUTEX_INITIALIZER;
static client_h g_handle;

static void *__work(void *args)
{
    int num = ((int)(long)args);
    client_h h = NULL;

    while (1) {
        pthread_mutex_lock(&gmtx);

        while (g_handle == NULL)
            pthread_cond_wait(&gcond, &gmtx);

        h = g_handle;
        g_handle = NULL;

        pthread_mutex_unlock(&gmtx);
        printf("[%d]%s\n",num , h->buf);
        free(h);
    }

    return NULL;
}

int main()
{
    int i;
    int ret;
    int sfd;
    struct sockaddr_in s_addr;
    client_h handle = NULL;
    void *ret_ptr;

    sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd == -1) {
        fprintf(stderr, "socket : %s", strerror(errno));
        return -1;
    }

    memset(&s_addr, 0, sizeof(struct sockaddr_in));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htons(INADDR_ANY);
    s_addr.sin_port = htons(PORT_NUMBER);

    ret = bind(sfd, (struct sockaddr *)&s_addr, sizeof(s_addr));
    if (ret == -1) {
        fprintf(stderr, "socket : %s", strerror(errno));
        return -1;
    }

    for (i = 0; i < THREAD_MAX; i++)
        pthread_create(&thr[i], 0, __work, ((void *)(long)i));

    while (1) {
        handle = (client_h)calloc(1, sizeof(struct _client_h));
        if (handle == NULL)
            continue;

        handle->buf_size = recvfrom(sfd, handle->buf, CLIENT_BUF_MAX, 0, 
                (struct sockaddr *)&handle->sock_addr, &handle->sock_len);
        if (handle->buf_size < 0) {
            pthread_mutex_unlock(&gmtx);
            perror("recvfrom");
            continue;
        }

        pthread_mutex_lock(&gmtx);
        g_handle = handle;
        handle = NULL;

        pthread_cond_signal(&gcond);

        pthread_mutex_unlock(&gmtx);
    }

    for (i = 0; i < THREAD_MAX; i++)
        pthread_join(thr[i], &ret_ptr);

    close(sfd);

    return 0;
}
