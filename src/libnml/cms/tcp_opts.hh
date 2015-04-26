#ifndef TCP_OPTS_HH
#define TCP_OPTS_HH

#ifdef WINTER_RAW
extern "C"
{
#endif

	/* Function shared by client and server to set desired options. */
	int set_tcp_socket_options(int socket_fd);
	int make_tcp_socket_nonblocking(int socket_fd);
	int make_tcp_socket_blocking(int socket_fd);

#ifdef WINTER_RAW
}
#endif

#endif /* TCP_OPTS_HH */

