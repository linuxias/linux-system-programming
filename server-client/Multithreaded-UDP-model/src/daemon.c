#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>
#include <fcntl.h>

static void make_child(void)
{
    switch (fork()) {
        case -1:
            exit(EXIT_FAILURE);
        case 0:
            break;
        default:
            exit(EXIT_SUCCESS);
    }
}

void s_daemon_become(void)
{
    int ret;
    int fd;
    int max_fd;

    make_child();

    ret = setsid();
    if (ret == -1)
        exit(EXIT_FAILURE);

    make_child();

    umask(0);

    ret = chdir("/");
    if (ret != 0)
        exit(EXIT_FAILURE);

    max_fd = sysconf(_SC_OPEN_MAX);
    if (max_fd == -1)
        max_fd = 8192;

    for (fd = 0; fd < max_fd; fd++)
        close(fd);

    close(STDIN_FILENO);

    fd = open("/dev/null", O_RDWR);
    if (fd != STDIN_FILENO)
        exit(EXIT_FAILURE);
    if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
        exit(EXIT_FAILURE);
    if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
        exit(EXIT_FAILURE);
}
