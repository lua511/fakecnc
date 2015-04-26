
#ifndef WINTER_RAW
#include "config.h"


#if defined(__KERNEL__)
#undef HAVE_SINCOS
#endif

#ifndef HAVE_SINCOS

#include "rtapi_math.h"
#include "sincos.h"

#ifndef HAVE___SINCOS

#include "posemath.h"

void sincos(double x, double *sx, double *cx)
{
    *sx = sin(x);
    *cx = cos(x);
}

#endif

#endif
#endif

#ifdef WINTER_RAW
#include <math.h>

void sincos(double x, double *sx, double *cx)
{
	*sx = sin(x);
	*cx = cos(x);
}
#endif

