#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_LEN 100

int main(int argc, char *argv[])
{
    fd_set rfds;
    struct timeval tv;
    int ret;
    char buf[BUF_LEN];

    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    ret = select(1, &rfds, NULL, NULL, &tv);
    if (ret == -1) {
        perror("select");
    } else if (ret) {
        printf("data is available now\n");
        read(STDIN_FILENO, buf, BUF_LEN);
        printf("%s", buf);
    } else {
        printf("No data within five second\n");
    }

    return 0;
}
