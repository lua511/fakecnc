#ifndef EMCGLB_H
#define EMCGLB_H

#include "config.h"             /* LINELEN */
#include "math.h"		/* M_PI */
#include "emcmotcfg.h"          /* EMCMOT_MAX_DIO */

#ifdef __cplusplus
extern "C" {
#endif

#define EMC_AXIS_MAX EMCMOT_MAX_AXIS

#define EMC_MAX_DIO EMCMOT_MAX_DIO
#define EMC_MAX_AIO EMCMOT_MAX_AIO


    extern char emc_inifile[LINELEN];

    extern char emc_nmlfile[LINELEN]; 

#define DEFAULT_RS274NGC_STARTUP_CODE ""
    extern char rs274ngc_startup_code[LINELEN];

    // there's also an emc_Debug function in emc/usr_intf/emcsh.cc
    extern int emc_debug;

    // EMC_DEBUG_* flag definitions moved to debugflags.h

    extern double emc_task_cycle_time;	

    extern double emc_io_cycle_time;

    extern int emc_task_interp_max_len;

    extern char tool_table_file[LINELEN];

    extern double traj_default_velocity;
    extern double traj_max_velocity;

    extern double axis_max_velocity[EMC_AXIS_MAX];
    extern double axis_max_acceleration[EMC_AXIS_MAX];

    extern struct EmcPose tool_change_position;
    extern unsigned char have_tool_change_position;

/*just used to keep track of unneccessary debug printing. */
    extern int taskplanopen;

    extern void emcInitGlobals();

#ifdef __cplusplus
}				/* matches extern "C" at top */
#endif

#endif
