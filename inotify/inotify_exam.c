#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/inotify.h>

#define INOTIFY_PATH "/tmp/"

int main(int argc, char *argv[])
{
	int ret;
	int fd;
	int wd;

	fd = inotify_init();
	if (fd < 0) {
		perror("inotify_init");
		exit(EXIT_FAILURE);
	}

	wd = inotify_add_watch(fd, INOTIFY_PATH, IN_MODIFY | IN_CREATE | IN_DELETE);
	if (wd < 0) {
		fprintf(stderr, "Failed to add watch [%s] [%s]", INOTIFY_PATH, strerror(errno));
		perror("inotify_add_watch");
		exit(EXIT_FAILURE);
	}

	ret = inotify_rm_watch(fd, wd);
	if (ret < 0) {
		fprintf(stderr, "Failed to rm watch [fd : %d] [wd : %d] [%s]", fd, wd, strerror(errno));
		perror("inotify_rm_watch");
		exit(EXIT_FAILURE);
	}

	return 0;
}
