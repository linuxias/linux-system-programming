#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <poll.h>
#include <errno.h>
#include <unistd.h>
#include <sys/inotify.h>

#define INOTIFY_PATH "/home/linuxias/Desktop/"
static void __handle_inotify_event(const struct inotify_event *event)
{
	if (event->mask & IN_ACCESS)
		printf("IN_ACCESS ");
	if (event->mask & IN_ATTRIB)
		printf("IN_ATTRIB ");
	if (event->mask & IN_CLOSE_NOWRITE)
		printf("IN_CLOSE_NOWRITE ");
	if (event->mask & IN_CLOSE_WRITE)
		printf("IN_CLOSE_WRITE ");
	if (event->mask & IN_CREATE)
		printf("IN_CREATE ");
	if (event->mask & IN_DELETE)
		printf("IN_DELETE ");
	if (event->mask & IN_ISDIR)
		printf("IN_ISDIR ");
	if (event->mask & IN_MODIFY)
		printf("IN_MODIFY ");
	if (event->mask & IN_MOVE_SELF)
		printf("IN_MOVE_SELF ");
	if (event->mask & IN_MOVED_FROM)
		printf("IN_MOVED_FROM ");
	if (event->mask & IN_MOVED_TO)
		printf("IN_MOVED_TO ");
	if (event->mask & IN_OPEN)
		printf("IN_OPEN ");

	if (event->len > 0)
		printf(": name = %s\n", event->name);
}

int main(int argc, char *argv[])
{
	int ret;
	int fd;
	int wd;
	char buf[4096] __attribute__ ((aligned(__alignof__(struct inotify_event))));
	char *ptr;
	ssize_t size;
	const struct inotify_event *event;

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

	while (1) {
		size = read(fd, buf, sizeof(buf));
		if (size == -1 && errno != EAGAIN) {
			perror("read");
			fprintf(stderr, "read : %s", strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (size <= 0)
			break;

		for (ptr = buf; ptr < buf + size; ptr += sizeof(struct inotify_event) + event->len) {
			event = (struct inotify_event *)ptr;
			__handle_inotify_event(event);
		}
	}

	ret = inotify_rm_watch(fd, wd);
	if (ret < 0) {
		fprintf(stderr, "Failed to rm watch [fd : %d] [wd : %d] [%s]", fd, wd, strerror(errno));
		perror("inotify_rm_watch");
		exit(EXIT_FAILURE);
	}

	return 0;
}
