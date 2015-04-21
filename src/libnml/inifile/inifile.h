#ifndef LINUXCNC_INIFILE_H
#define LINUXCNC_INIFILE_H

#include <stdio.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern const char *iniFind(FILE *fp, const char *tag, const char *section);
extern const int iniFindInt(FILE *fp, const char *tag, const char *section, int *result);
extern const int iniFindDouble(FILE *fp, const char *tag, const char *section, double *result);
extern int TildeExpansion(const char *file, char *path, size_t size);

#ifdef __cplusplus
}
#endif
#endif

