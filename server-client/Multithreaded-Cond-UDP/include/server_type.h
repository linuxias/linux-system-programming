#ifndef __SERVER_TYPE_H__
#define __SERVER_TYPE_H__

#define CLIENT_BUF_MAX 1500

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct _client_h *client_h;

struct _client_h {
    struct sockaddr_in sock_addr;
    socklen_t sock_len;
    uint8_t buf[CLIENT_BUF_MAX];
    ssize_t buf_size;
};

#endif /* __SERVER_TYPE_H__ */
