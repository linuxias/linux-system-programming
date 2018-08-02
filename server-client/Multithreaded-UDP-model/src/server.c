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
#define THREAD_MAX 10

static void *__work(void *args)
{
    client_h handle = (client_h)args;

    printf("%s\n", handle->buf);
    sleep(1);
    return NULL;
}

int main()
{
    int ret;
    int sfd;
    struct sockaddr_in s_addr;
    struct sockaddr_in c_addr;
    client_h handle = NULL;
    pthread_t thr;

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

    while (1) {
        memset(&c_addr, 0, sizeof(struct sockaddr_in));

        handle = NULL;
        handle = (client_h)calloc(1, sizeof(struct _client_h));
        if (handle == NULL)
            continue;

        handle->buf_size = recvfrom(sfd, handle->buf, CLIENT_BUF_MAX, 0, 
                (struct sockaddr *)&handle->sock_addr, &handle->sock_len);
        if (handle->buf_size < 0)
            continue;

        ret = pthread_create(&thr, 0, __work, handle);
        if (ret < 0) {
            perror("pthread_create");
            continue;
        }

        pthread_detach(thr);
    }

    return 0;
}
