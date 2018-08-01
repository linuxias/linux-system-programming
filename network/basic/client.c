#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP 127.0.0.1 //IP
#define SERVER_PORT 8888    //PORT
#define BUF_SIZE 1024

#define exit_if(SRC, DST, FUNC) \
    do { \
        printf("%d\n", SRC); \
        if (SRC == DST) { \
            perror(FUNC); \
            exit(1); \
        } \
    } while(0);

int main()
{
    int ret;
    int client_sock;
    struct sockaddr_in server_addr;
    char buf[BUF_SIZE];

    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    exit_if(client_sock, -1, "socket");

    memset(&server_addr, 0x00, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    ret = connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
    exit_if(ret, -1, "connect");

    while (1) {
        memset(&buf, 0x00, sizeof(buf));
        fgets(buf, sizeof(buf), stdin);

        ret = strcmp(buf, "quit");
        if (ret == 0) {
            close(client_sock);
            return 0;
        }

        write(client_sock, buf, sizeof(buf));
    }

    return 0;
}
