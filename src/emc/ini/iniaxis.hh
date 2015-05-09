#ifndef INIAXIS_HH
#define INIAXIS_HH

#ifndef WINTER_RAW
#include "emc.hh"		// EMC_AXIS_STAT
#else
#include "check_iniaxis.hh"
#endif

/* initializes axis modules from ini file */
extern int iniAxis(int axis, const char *filename);

/* dump axis stat to ini file */
extern int dumpAxis(int axis, const char *filename,
		    EMC_AXIS_STAT * status);
#endif

