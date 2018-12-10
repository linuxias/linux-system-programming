#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>

int main(int argc, char* argv[]) {
    char buf[100];
    int fd, i, poll_num;
    int *wd;
    nfds_t nfds;
    struct pollfd fds[2];
    int pipe_fd[2];
    pid_t pid;

    if (pipe(pipe_fd) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    nfds = 2;

    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    fds[1].fd = pipe_fd[0];
    fds[1].events = POLLIN;

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipe_fd[0]);
        while (1) {
            write(pipe_fd[1], "TEST", 5);
            sleep(5);
        }
    } else {
        close(pipe_fd[1]);
        while (1) {
            poll_num = poll(fds, nfds, -1);
            if (poll_num == -1) {
                if (errno == EINTR)
                    continue;
                perror("poll");
                exit(EXIT_FAILURE);
            }

            if (poll_num > 0) {
                if (fds[0].revents & POLLIN) {
                    if (read(STDIN_FILENO, &buf, 100) > 0) {
                        printf("%s\n", buf);
                        memset(buf, 0, 100);
                    }
                }
                if (fds[1].revents & POLLIN) {
                    read(pipe_fd[0], buf, sizeof(buf));
                    printf("%s\n", buf);
                }
            }
        }
    }

    close(fd);
    free(wd);

    return 0;
}
