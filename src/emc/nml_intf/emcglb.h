#ifndef EMCGLB_H
#define EMCGLB_H

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_EMC_INIFILE "emc.ini"
#define DEFAULT_EMC_NMLFILE EMC2_DEFAULT_NMLFILE

    extern char emc_inifile[LINELEN];

    extern char emc_nmlfile[LINELEN]; 

#ifdef __cplusplus
}				/* matches extern "C" at top */
#endif

#endif
