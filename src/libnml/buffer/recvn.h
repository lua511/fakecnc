#ifndef RECVN_H
#define RECVN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>		/* size_t */

    int recvn(int fd, void *vptr, int n, int flags, double timeout,
	int *bytes_read_ptr);
    extern int recvn_timedout;
    extern int print_recvn_timeout_errors;
#ifdef __cplusplus
};
#endif

#endif /* RECVN_H */

