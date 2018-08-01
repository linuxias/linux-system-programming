#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <unistd.h>

#define exit_if(SRC, DST, FUNC) \
    do { \
        if (SRC == DST) { \
            perror(FUNC); \
            exit(1); \
        } \
    } while(0);

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    int ret;
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    int len;
    char buf[BUF_SIZE] = {0,};

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    exit_if(server_sock, -1, "socket");

    memset(&server_addr, 0x00, sizeof(struct sockaddr_in));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    ret = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    exit_if(ret, -1, "bind");

    ret = listen(server_sock, 10);
    exit_if(ret, -1, "listen");

    len = sizeof(struct sockaddr_in);
    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &len);
    exit_if(client_sock, -1, "accept");

    while(1) {
        memset(buf, 0x00, sizeof(buf));
        ret = read(client_sock, buf, BUF_SIZE);
        if (ret == -1) {
            printf("read fail\n");
            continue;
        }

        printf("%s\n", buf);
    }

    return 0;
}
