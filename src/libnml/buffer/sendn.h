#ifndef WRITEN_H
#define WRITEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>		/* size_t */

    int sendn(int fd, const void *vptr, int n, int flags, double timeout);

#ifdef __cplusplus
};
#endif

#endif /* WRITEN_H */

