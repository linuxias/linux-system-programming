#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>

#define RCV_SOCK 0
#define SND_SOCK 1

#define MAX_RETRY_CNT 5
#define WRITE_TIMEOUT 20 /* milliseconds*/

static int read_buf_size = -1;
static int write_buf_size = -1;

static int __get_socket_buffer_size(int fd, int optname)
{
	int err;
	int size;
	socklen_t len = sizeof(int);

	errno = 0;
	err = getsockopt(fd, SOL_SOCKET, optname, (void *)&size, &len);
	if (err != 0) {
		printf("getsockopt [%d]", errno);
		return -1;
	}

	return size;
}

int socket_create_pair(int *fd)
{
	int ret_fd[2];
	int err;

	errno = 0;
	err = socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, ret_fd);
	if (err != 0) {
		printf("socketpair [%d]", errno);
		return errno;
	}

	fd[RCV_SOCK] = ret_fd[RCV_SOCK];
	fd[SND_SOCK] = ret_fd[SND_SOCK];

	read_buf_size = __get_socket_buffer_size(ret_fd[RCV_SOCK], SO_RCVBUF);
	read_buf_size = __get_socket_buffer_size(ret_fd[SND_SOCK], SO_SNDBUF);
	if (read_buf_size == -1 || write_buf_size == -1) {
		close(ret_fd[RCV_SOCK]);
		close(ret_fd[SND_SOCK]);
		return -1;
	}

	return 0;
}

int socket_write(int fd, const char *buffer, unsigned int nbytes)
{
	unsigned int left = nbytes;
	int retry_cnt = 0;
	ssize_t nb;
	const struct timespec SLEEP_TIME = { 0, 20 * 1000 * 1000 };

	/* If you set socket to blocking mode, you need poll */
	/*
	   struct pollfd fds[1];
	   fds[0].fd = fd;
	   fds[0].events = POLLOUT;
	   fds[0].revents = 0;

	   ret = poll(fds, 1, WRITE_TIMEOUT);
	   if (ret == 0) {
		   printf("fd %d poll timeout", fd);
		   return -1;
	   }
	 */

	while (left && (retry_cnt < MAX_RETRY_CNT)) {
		errno = 0;
		nb = write(fd, buffer, left);
		if (nb == -1) {
			if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
				printf("errno [%d], sleep and retry", errno);
				retry_cnt++;
				nanosleep(&SLEEP_TIME, 0);
				continue;
			}
			printf("errno [%d] fd [%d]", errno, fd);
			return -1;
		}

			printf("error fd [%d] errno [%d]", fd, errno);
			return -1;
		}

		left -= nb;
		buffer += nb;
		retry_cnt = 0;
	}

	if (left != 0) {
		printf("error fd [%d], retry_cnt [%d]", fd, retry_cnt);
		return -1;
	}

	return 0;
}

int socket_write_string(int fd, const char *buffer, int string_len)
{
	int ret;

	ret = socket_write(fd, (char *)&string_len, sizeof(string_len));
	if (ret != 0) {
		printf("write string_len fail");
		return ret;
	}

	if (string_len > 0) {
		ret = socket_write(fd, buffer, string_len);
		if (ret != 0) {
			printf("write string fail");
			return ret;
		}
	}

	return 0;
}

int socket_read(int fd, char *buffer, unsigned int nbytes)
{
	unsigned int left = nbytes;
	ssize_t nb;
	int retry_cnt = 0;
	const struct timespec SLEEP_TIME = { 0, 20 * 1000 * 1000 };

	while (left && (retry_cnt < MAX_RETRY_CNT)) {
		errno = 0;
		nb = read(fd, buffer, left);
		if (nb == 0) {
			printf("read socket - EOF, fd close [%d]", fd);
			return -1;
		} else if (nb == -1) {
			if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
				printf("errno [%d], sleep and retry", errno);
				retry_cnt++;
				nanosleep(&SLEEP_TIME, 0);
				continue;
			}
			printf("errno [%d] fd [%d]", errno, fd);
			return -1;
		}
		left -= nb;
		buffer += nb;
		retry_cnt = 0;
	}

	if (left != 0) {
		printf("error fd [%d] retry_cnt [%d]", fd, retry_cnt);
		return -1;
	}

	return 0;
}

int ipc_socket_read_string(int fd, char **buffer, int *string_len)
{
	ret = socket_read(fd, (char *)string_len, sizeof(*string_len));
	if (ret != 0) {
		printf("read socket fail");
		return ret;
	}

	if (*string_len > 0 && *string_len < read_buf_size) {
		*buffer = (char *)calloc(*string_len, sizeof(char));
		if (*buffer == NULL) {
			printf("OOM");
			return -1;
		}

		ret = socket_read(fd, *buffer, *string_len);
		if (ret != 0) {
			printf("read socket fail");
			return ret;
		}
	} else {
		printf("Invalid string len %d", *string_len);
		return -1;
	}

	return 0;
}
