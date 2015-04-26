#ifndef SINCOS_H
#define SINCOS_H

#ifndef WINTER_RAW

#include "config.h"
/*
  for each platform that has built-in support for the sincos function,
  that should be discovered by ./configure
*/

/* testing for sincos now supported by ./configure */
#ifdef HAVE___SINCOS
#define sincos __sincos
#endif


/*
  all other platforms will not have __sincos, and will
  get the declaration for the explicit function
*/

#ifndef HAVE___SINCOS

extern void sincos(double x, double *sx, double *cx);

#endif

#endif /* #ifndef SINCOS_H */

#ifdef WINTER_RAW
#ifdef __cplusplus
extern "C"
{
#endif
	extern void sincos(double x, double *sx, double *cx);
#if __cplusplus
}
#endif
#endif // !WINTER_RAW


#endif

