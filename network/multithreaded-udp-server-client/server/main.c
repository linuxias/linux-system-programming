#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>


#define SERVER_PORT 7777

int main()
{
	int ret;
	int sfd;
	socklen_t s_len;
	struct sockaddr_in s_addr;
	char buf[100];

	sfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sfd < 0)
		return -1;

	memset(&s_addr, 0, sizeof(struct sockaddr_in));

	s_addr.sin_family =  AF_INET;
	s_addr.sin_addr.s_addr = INADDR_ANY;
	s_addr.sin_port = htons(SERVER_PORT);
	s_len = sizeof(s_addr);

	ret = bind(sfd, (struct sockaddr *)&s_addr, s_len);
	if (ret == -1)
		return -1;

	while (1) {
	}

	return 0;
}
