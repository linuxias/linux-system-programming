#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <unistd.h>

static void handle_events(int fd, int *wd, int argc, char* argv[]) {
    int i;
    ssize_t len;
    char *ptr;
    char buf[4096] __attribute__ ((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;

    for (;;) {
        len = read(fd, buf, sizeof buf);
        if (len == -1 && errno != EAGAIN) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        if (len <= 0)
            break;

        for (ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len) {
            event = (const struct inotify_event *) ptr;

            for (i = 1; i < argc; ++i) {
                if (wd[i] == event->wd) {
                    printf("%s ", argv[i]);
                    break;
                }
            }

            if (event->mask & IN_OPEN)
                printf("IN_OPEN");
            if (event->mask & IN_CLOSE_NOWRITE)
                printf("IN_CLOSE_NOWRITE");
            if (event->mask & IN_CLOSE_WRITE)
                printf("IN_CLOSE_WRITE");
            if (event->mask & IN_MODIFY)
                printf("IN_MODIFY");
            if (event->mask & IN_CLOSE_WRITE)
                printf("IN_CLOSE_WRITE");
            if (event->mask & IN_IGNORED)
                printf("IN_IGNORED");
            printf("\n");
        }
    }
}

int main(int argc, char* argv[]) {
    char buf[100];
    int fd, i, poll_num;
    int *wd;
    nfds_t nfds;
    struct pollfd fds[2];

    if (argc < 2) {
        printf("Usage: %s PATH [PATH ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1) {
        perror("inotify_init1");
        exit(EXIT_FAILURE);
    }

    wd = calloc(argc, sizeof(int));
    if (wd == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    for (i = 1; i < argc; i++) {
        wd[i] = inotify_add_watch(fd, argv[i], IN_OPEN | IN_CLOSE | IN_CLOSE_WRITE | IN_MODIFY | IN_IGNORED);
        if (wd[i] == -1) {
            fprintf(stderr, "Cannot watch '%s'\n", argv[i]);
            perror("inotify_add_watch");
            exit(EXIT_FAILURE);
        }
    }

    nfds = 2;

    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    fds[1].fd = fd;
    fds[1].events = POLLIN;

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
                handle_events(fd, wd, argc, argv);
            }
        }
    }

    close(fd);
    free(wd);

    return 0;
}
