#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MSG_SIZE 128

int main(int argc, char *argv[])
{
    int fd;
    char *group;
    int port;
    int nbytes;
    int idx = 0;
    char message[MSG_SIZE];
    struct sockaddr_in addr;

    if (argc != 3) {
        printf("./server ip port\n");
        exit(EXIT_FAILURE);
    }

    group = argv[1];
    port = atoi(argv[2]);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(group);
    addr.sin_port = htons(port);

    while (1) {
        snprintf(message, 128, "HELLO WORLD %d\n", ++idx);
        nbytes = sendto(fd, message, strlen(message), 0, (struct sockaddr*) &addr, sizeof(addr));
        if (nbytes < 0) {
            perror("sendto");
            exit(EXIT_FAILURE);
        }
        if (idx > 1000000)
            idx = 0;
        usleep(10);
    }

    return 0;
}
