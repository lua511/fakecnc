#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "tcp_opts.hh"

int main(int v, char* c[])
{
	int socket_fd;
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("create socket fail\n");
	}
	printf("make nonb: [%d]\n", make_tcp_socket_nonblocking(socket_fd));
	printf("make bloc: [%d]\n", make_tcp_socket_blocking(socket_fd));
	close(socket_fd);
	return 0;
}
