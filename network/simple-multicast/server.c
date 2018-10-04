#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define MSG_SIZE 128

int main(int argc, char *argv[])
{
    int ret;
    int fd;
    int reuse = 1;
    int addrlen;
    int nbytes;
    int port;
    char* group;
    char message[MSG_SIZE];
    struct sockaddr_in addr;
    struct ip_mreq mreq;

    if (argc != 3) {
        printf("./client ip port\n");
        exit(EXIT_FAILURE);
    }

    group = argv[1];
    port = atoi(argv[2]);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
 
    ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (ret == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    mreq.imr_multiaddr.s_addr = inet_addr(group);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    ret = setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&mreq, sizeof(mreq));
    if (ret == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    int idx = 0;
    while (1) {
        addrlen = sizeof(addr);
        nbytes = recvfrom(fd, message, MSG_SIZE, 0, (struct sockaddr *)&addr, &addrlen);
        if (nbytes < 0) {
//            perror("recvfrom");
//            exit(EXIT_FAILURE);
        }
        message[nbytes] = '\0';
        printf("%s", message);
    }

    return 0;
}
