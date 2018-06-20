#include <stdio.h>
#include <sys/inotify.h>

int main(int argc, char *argv[])
{
	int fd;

	fd = inotify_init();

	return 0;
}
