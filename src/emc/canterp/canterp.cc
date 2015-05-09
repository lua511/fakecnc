#include <stdio.h>		// FILE, fopen(), fclose()
#include <string.h>		// strcpy()
#include <ctype.h>		// isspace()
#include <limits.h>
#include <algorithm>
#include "config.h"
#include "emc/nml_intf/interp_return.hh"
#include "emc/nml_intf/canon.hh"
#include "emc/rs274ngc/interp_base.hh"

static char the_command[LINELEN] = { 0 };	// our current command
static char the_command_name[LINELEN] = { 0 };	// just the name part
static char the_command_args[LINELEN] = { 0 };	// just the args part

class Canterp : public InterpBase {
public:
    Canterp () : f(0) {}
    char *error_text(int errcode, char *buf, size_t buflen);
    char *stack_name(int index, char *buf, size_t buflen);
    char *line_text(char *buf, size_t buflen);
    char *file_name(char *buf, size_t buflen);
    size_t line_length();
    int sequence_number();
    int ini_load(const char *inifile);
    int init();
    int execute();
    int execute(const char *line);
    int execute(const char *line, int line_number);
    int synch();
    int exit();
    int open(const char *filename);
    int read();
    int read(const char *line);
    int close();
    int reset();
    int line();
    int call_level();
    char *command(char *buf, size_t buflen);
    char *file(char *buf, size_t buflen);
    int on_abort(int reason, const char *message);
    void active_g_codes(int active_gcodes[ACTIVE_G_CODES]);
    void active_m_codes(int active_mcodes[ACTIVE_M_CODES]);
    void active_settings(double active_settings[ACTIVE_SETTINGS]);
    void set_loglevel(int level);
    FILE *f;
    char filename[PATH_MAX];
};

char *Canterp::error_text(int errcode, char *buf, size_t buflen) {
    if(errcode < INTERP_MIN_ERROR) snprintf(buf, buflen, "OK %d", errcode);
    else snprintf(buf, buflen, "ERROR %d", errcode);
    return buf;
}

char *Canterp::stack_name(int index, char *buf, size_t buflen) {
    snprintf(buf, buflen, "<stack %d>", index);
    return buf;
}

int Canterp::ini_load(const char *inifile) {
    return 0;
}

/*
  We expect lines like this:

  {ws}1<white>N1<ws>cmd{ws}({ws}{arg1{ws}}){extra}

 */

static char *skipwhite(char *ptr)
{
    while (isspace(*ptr))
	ptr++;
    return ptr;
}

static char *findwhite(char *ptr)
{
    while (!isspace(*ptr) && 0 != *ptr)
	ptr++;
    return ptr;
}

static int canterp_parse(char *buffer)
{
    char *ptr = buffer;
    char *cmd_ptr = the_command;
    char *name_ptr = the_command_name;
    char *args_ptr = the_command_args;
    char *last_quote_ptr;
    int inquote;

    *cmd_ptr = 0;
    *name_ptr = 0;
    *args_ptr = 0;

    // skip leading white space, return if nothing else found
    if (0 == *(ptr = skipwhite(ptr)))
	return 0;

    // ---cut here if no leading line number, N-number columns---

    // skip the first column, return if nothing else found
    if (0 == *(ptr = findwhite(ptr)))
	return 0;

    // skip following white space, return if nothing else found
    if (0 == *(ptr = skipwhite(ptr)))
	return 0;

    // skip the second column, return if nothing else found
    if (0 == *(ptr = findwhite(ptr)))
	return 0;

    // skip following white space, return if nothing else found
    if (0 == *(ptr = skipwhite(ptr)))
	return 0;

    // ---cut to here---

    // we got something; store the name, up to space or the '('
    while (!isspace(*ptr) && '(' != *ptr && 0 != *ptr) {
	*name_ptr++ = *ptr;
	*cmd_ptr++ = *ptr++;
    }
    if (isspace(*ptr))
	ptr = skipwhite(ptr);

    if (0 == *ptr) {
	// no parens, just a command name
	*name_ptr = 0;
	*cmd_ptr = 0;
	return 0;
    }

    if ('(' != *ptr) {
	// we're missing the '(', so flag an error
	*name_ptr = 0;
	*cmd_ptr = 0;
	return INTERP_ERROR;
    }
    // we got the '(', so keep going
    *name_ptr = 0;		// terminate the name
    *cmd_ptr++ = *ptr++;	// add the '(' to the full command

    /*
       now we're at the args; skip first and last quotes when building
       args_ptr, and make commas spaces for easy parsing later
     */
    last_quote_ptr = 0;
    inquote = 0;
    while (')' != *ptr && 0 != *ptr) {
	if ('"' == *ptr) {
	    if (!inquote) {
		// here's the first quote, so suppress it in args_ptr
		inquote = 1;
		*cmd_ptr++ = *ptr++;
		continue;
	    }
	    /*
	       else it's a quote-in-quote, so mark it as the last so we
	       can delete it, thus handling any internal quotes, perhaps
	       used for inch marks
	     */
	    last_quote_ptr = args_ptr;
	}
	*args_ptr++ = (*ptr == ',' ? ' ' : *ptr);
	*cmd_ptr++ = *ptr++;
    }
    if (0 == *ptr) {
	// finished args without ')', so error
	*args_ptr = 0;
	*cmd_ptr = 0;
	return INTERP_ERROR;
    }
    if (0 != last_quote_ptr)
	*last_quote_ptr = 0;
    *args_ptr = 0;
    *cmd_ptr++ = ')';
    *cmd_ptr = 0;

    return 0;
}

int Canterp::read(const char *line) {
    return canterp_parse((char *) line);
}

int Canterp::read() {
    char buf[LINELEN];
    if(!f) return INTERP_ERROR;
    if(!fgets(buf, sizeof(buf), f)) return INTERP_ENDFILE;
    return canterp_parse(buf);
}

int Canterp::execute(const char *line) {
    int retval;
    double d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11;
    int i1, ln=-1;
    char s1[256];

    if (line) {
	retval = canterp_parse((char *) line);
	if (retval)
	    return retval;
    }

    // a blank line
    if (strlen(the_command_name) == 0) return INTERP_OK;

    if (!strcmp(the_command_name, "STRAIGHT_FEED")) {
	if (9 != sscanf(the_command_args, "%lf %lf %lf %lf %lf %lf %lf %lf %lf",
			&d1, &d2, &d3, &d4, &d5, &d6, &d7, &d8, &d9)) {
	    return INTERP_ERROR;
	}
	STRAIGHT_FEED(ln, d1, d2, d3, d4, d5, d6, d7, d8, d9);
	return 0;
    }

    if (!strcmp(the_command_name, "ARC_FEED")) {
	if (12 != sscanf(the_command_args,
			"%lf %lf %lf %lf %d %lf %lf %lf %lf %lf %lf %lf",
			&d1, &d2, &d3, &d4, &i1, &d5, &d6, &d7, &d8, &d9, &d10, &d11)) {
	    return INTERP_ERROR;
	}
	ARC_FEED(ln, d1, d2, d3, d4, i1, d5, d6, d7, d8, d9, d10, d11);
	return 0;
    }

    if (!strcmp(the_command_name, "STRAIGHT_TRAVERSE")) {
	if (9 != sscanf(the_command_args, "%lf %lf %lf %lf %lf %lf %lf %lf %lf",
			&d1, &d2, &d3, &d4, &d5, &d6, &d7, &d8, &d9)) {
	    return INTERP_ERROR;
	}
	STRAIGHT_TRAVERSE(ln, d1, d2, d3, d4, d5, d6, d7, d8, d9);
	return 0;
    }

    if (!strcmp(the_command_name, "STRAIGHT_PROBE")) {
	if (6 != sscanf(the_command_args, "%lf %lf %lf %lf %lf %lf %lf %lf %lf",
			&d1, &d2, &d3, &d4, &d5, &d6, &d7, &d8, &d9)) {
	    return INTERP_ERROR;
	}
	STRAIGHT_PROBE(ln, d1, d2, d3, d4, d5, d6, d7, d8, d9, 0);
	return 0;
    }

    if (!strcmp(the_command_name, "USE_LENGTH_UNITS")) {
	if (!strcmp(the_command_args, "CANON_UNITS_MM")) {
	    USE_LENGTH_UNITS(CANON_UNITS_MM);
	    return 0;
	}
	if (!strcmp(the_command_args, "CANON_UNITS_CM")) {
	    USE_LENGTH_UNITS(CANON_UNITS_MM);
	    return 0;
	}
	if (!strcmp(the_command_args, "CANON_UNITS_INCHES")) {
	    USE_LENGTH_UNITS(CANON_UNITS_INCHES);
	    return 0;
	}
	return INTERP_ERROR;
    }

#if 0
    if (!strcmp(the_command_name, "SET_ORIGIN_OFFSETS")) {
	if (6 != sscanf(the_command_args, "%lf %lf %lf %lf %lf %lf %lf %lf %lf",
			&d1, &d2, &d3, &d4, &d5, &d6, &d7, &d8, &d9)) {
	    return INTERP_ERROR;
	}
	SET_ORIGIN_OFFSETS(d1, d2, d3, d4, d5, d6, d7, d8, d9);
	return 0;
    }
#endif

    if (!strcmp(the_command_name, "SET_FEED_REFERENCE")) {
	if (!strcmp(the_command_args, "CANON_WORKPIECE")) {
	    SET_FEED_REFERENCE(CANON_WORKPIECE);
	    return 0;
	}
	if (!strcmp(the_command_args, "CANON_XYZ")) {
	    SET_FEED_REFERENCE(CANON_XYZ);
	    return 0;
	}
	return INTERP_ERROR;
    }

    if (!strcmp(the_command_name, "SELECT_PLANE")) {
	if (!strcmp(the_command_args, "CANON_PLANE_XY")) {
	    SELECT_PLANE(CANON_PLANE_XY);
	    return 0;
	}
	if (!strcmp(the_command_args, "CANON_PLANE_YZ")) {
	    SELECT_PLANE(CANON_PLANE_YZ);
	    return 0;
	}
	if (!strcmp(the_command_args, "CANON_PLANE_XZ")) {
	    SELECT_PLANE(CANON_PLANE_XZ);
	    return 0;
	}
	return INTERP_ERROR;
    }

    if (!strcmp(the_command_name, "COMMENT")) {
	COMMENT(the_command_args);
	return 0;
    }

    if (!strcmp(the_command_name, "MIST_OFF")) {
	MIST_OFF();
	return 0;
    }

    if (!strcmp(the_command_name, "FLOOD_OFF")) {
	FLOOD_OFF();
	return 0;
    }

    if (!strcmp(the_command_name, "MIST_ON")) {
	MIST_ON();
	return 0;
    }

    if (!strcmp(the_command_name, "FLOOD_ON")) {
	FLOOD_ON();
	return 0;
    }

#if 0
    if (!strcmp(the_command_name, "USE_TOOL_LENGTH_OFFSET")) {
	if (1 != sscanf(the_command_args, "%lf %lf %lf", &d1, &d2, &d3)) {
	    return INTERP_ERROR;
	}
	USE_TOOL_LENGTH_OFFSET(d1, d2, d3);
	return 0;
    }
#endif

    if (!strcmp(the_command_name, "SET_FEED_RATE")) {
	if (1 != sscanf(the_command_args, "%lf", &d1)) {
	    return INTERP_ERROR;
	}
	SET_FEED_RATE(d1);
	return 0;
    }

#if 0
    if (!strcmp(the_command_name, "SET_TRAVERSE_RATE")) {
	if (1 != sscanf(the_command_args, "%lf", &d1)) {
	    return INTERP_ERROR;
	}
	SET_TRAVERSE_RATE(d1);
	return 0;
    }
#endif

    if (!strcmp(the_command_name, "SELECT_POCKET")) {
	if (1 != sscanf(the_command_args, "%d", &i1)) {
	    return INTERP_ERROR;
	}
	SELECT_POCKET(i1, i1);
	return 0;
    }

    if (!strcmp(the_command_name, "CHANGE_TOOL")) {
	if (1 != sscanf(the_command_args, "%d", &i1)) {
	    return INTERP_ERROR;
	}
	CHANGE_TOOL(i1);
	return 0;
    }

    if (!strcmp(the_command_name, "DWELL")) {
	if (1 != sscanf(the_command_args, "%lf", &d1)) {
	    return INTERP_ERROR;
	}
	DWELL(d1);
	return 0;
    }

#if 0
    if (!strcmp(the_command_name, "SPINDLE_RETRACT")) {
	SPINDLE_RETRACT();
	return 0;
    }

    if (!strcmp(the_command_name, "SPINDLE_RETRACT_TRAVERSE")) {
	SPINDLE_RETRACT_TRAVERSE();
	return 0;
    }

    if (!strcmp(the_command_name, "LOCK_SPINDLE_Z")) {
	LOCK_SPINDLE_Z();
	return 0;
    }

    if (!strcmp(the_command_name, "USE_SPINDLE_FORCE")) {
	USE_SPINDLE_FORCE();
	return 0;
    }

    if (!strcmp(the_command_name, "USE_NO_SPINDLE_FORCE")) {
	USE_NO_SPINDLE_FORCE();
	return 0;
    }

    if (!strcmp(the_command_name, "CLAMP_AXIS")) {
	if (!strcmp(the_command_args, "CANON_AXIS_X")) {
	    CLAMP_AXIS(CANON_AXIS_X);
	    return 0;
	}
	if (!strcmp(the_command_args, "CANON_AXIS_Y")) {
	    CLAMP_AXIS(CANON_AXIS_Y);
	    return 0;
	}
	if (!strcmp(the_command_args, "CANON_AXIS_Z")) {
	    CLAMP_AXIS(CANON_AXIS_Z);
	    return 0;
	}
	if (!strcmp(the_command_args, "CANON_AXIS_A")) {
	    CLAMP_AXIS(CANON_AXIS_A);
	    return 0;
	}
	if (!strcmp(the_command_args, "CANON_AXIS_B")) {
	    CLAMP_AXIS(CANON_AXIS_B);
	    return 0;
	}
	if (!strcmp(the_command_args, "CANON_AXIS_C")) {
	    CLAMP_AXIS(CANON_AXIS_C);
	    return 0;
	}
	return INTERP_ERROR;
    }

    if (!strcmp(the_command_name, "UNCLAMP_AXIS")) {
	if (!strcmp(the_command_args, "CANON_AXIS_X")) {
	    UNCLAMP_AXIS(CANON_AXIS_X);
	    return 0;
	}
	if (!strcmp(the_command_args, "CANON_AXIS_Y")) {
	    UNCLAMP_AXIS(CANON_AXIS_Y);
	    return 0;
	}
	if (!strcmp(the_command_args, "CANON_AXIS_Z")) {
	    UNCLAMP_AXIS(CANON_AXIS_Z);
	    return 0;
	}
	if (!strcmp(the_command_args, "CANON_AXIS_A")) {
	    UNCLAMP_AXIS(CANON_AXIS_A);
	    return 0;
	}
	if (!strcmp(the_command_args, "CANON_AXIS_B")) {
	    UNCLAMP_AXIS(CANON_AXIS_B);
	    return 0;
	}
	if (!strcmp(the_command_args, "CANON_AXIS_C")) {
	    UNCLAMP_AXIS(CANON_AXIS_C);
	    return 0;
	}
	return INTERP_ERROR;
    }

    if (!strcmp(the_command_name, "SET_CUTTER_RADIUS_COMPENSATION")) {
	if (1 != sscanf(the_command_args, "%lf", &d1)) {
	    return INTERP_ERROR;
	}
	SET_CUTTER_RADIUS_COMPENSATION(d1);
	return 0;
    }

    if (!strcmp(the_command_name, "START_CUTTER_RADIUS_COMPENSATION")) {
	if (1 != sscanf(the_command_args, "%d", &i1)) {
	    return INTERP_ERROR;
	}
	START_CUTTER_RADIUS_COMPENSATION(i1);
	return 0;
    }

    if (!strcmp(the_command_name, "STOP_CUTTER_RADIUS_COMPENSATION")) {
	STOP_CUTTER_RADIUS_COMPENSATION();
	return 0;
    }
#endif

    if (!strcmp(the_command_name, "START_SPEED_FEED_SYNCH")) {
	if (2 != sscanf(the_command_args, "%lf %d", &d1, &i1)) {
            return INTERP_ERROR;
        }
	START_SPEED_FEED_SYNCH(d1, i1);
	return 0;
    }

    if (!strcmp(the_command_name, "STOP_SPEED_FEED_SYNCH")) {
	STOP_SPEED_FEED_SYNCH();
	return 0;
    }

    if (!strcmp(the_command_name, "SET_SPINDLE_SPEED")) {
	if (1 != sscanf(the_command_args, "%lf", &d1)) {
	    return INTERP_ERROR;
	}
	SET_SPINDLE_SPEED(d1);
	return 0;
    }

    if (!strcmp(the_command_name, "START_SPINDLE_CLOCKWISE")) {
	START_SPINDLE_CLOCKWISE();
	return 0;
    }

    if (!strcmp(the_command_name, "START_SPINDLE_COUNTERCLOCKWISE")) {
	START_SPINDLE_COUNTERCLOCKWISE();
	return 0;
    }

    if (!strcmp(the_command_name, "STOP_SPINDLE_TURNING")) {
	STOP_SPINDLE_TURNING();
	return 0;
    }

    if (!strcmp(the_command_name, "ORIENT_SPINDLE")) {
	if (2 != sscanf(the_command_args, "%lf %s", &d1, s1)) {
	    return INTERP_ERROR;
	}
	if (!strcmp(s1, "CANON_CLOCKWISE")) {
	    ORIENT_SPINDLE(d1, CANON_CLOCKWISE);
	    return 0;
	}
	if (!strcmp(s1, "CANON_COUNTERCLOCKWISE")) {
	    ORIENT_SPINDLE(d1, CANON_COUNTERCLOCKWISE);
	    return 0;
	}
	return INTERP_ERROR;
    }

    if (!strcmp(the_command_name, "DISABLE_SPEED_OVERRIDE")) {
	DISABLE_SPEED_OVERRIDE();
	return 0;
    }

    if (!strcmp(the_command_name, "DISABLE_FEED_OVERRIDE")) {
	DISABLE_FEED_OVERRIDE();
	return 0;
    }

    if (!strcmp(the_command_name, "ENABLE_SPEED_OVERRIDE")) {
	ENABLE_SPEED_OVERRIDE();
	return 0;
    }

    if (!strcmp(the_command_name, "ENABLE_FEED_OVERRIDE")) {
	ENABLE_FEED_OVERRIDE();
	return 0;
    }

    if (!strcmp(the_command_name, "PROGRAM_STOP")) {
	PROGRAM_STOP();
	return 0;
    }

    if (!strcmp(the_command_name, "OPTIONAL_PROGRAM_STOP")) {
	OPTIONAL_PROGRAM_STOP();
	return 0;
    }

    if (!strcmp(the_command_name, "PROGRAM_END")) {
	PROGRAM_END();
	return 0;
    }

    if (!strcmp(the_command_name, "PALLET_SHUTTLE")) {
	PALLET_SHUTTLE();
	return 0;
    }

    if (!strcmp(the_command_name, "SET_MOTION_CONTROL_MODE")) {
	if (!strcmp(the_command_args, "CANON_EXACT_PATH")) {
	    SET_MOTION_CONTROL_MODE(CANON_EXACT_PATH, 0);
	    return 0;
	}
	if (!strcmp(the_command_args, "CANON_EXACT_STOP")) {
	    SET_MOTION_CONTROL_MODE(CANON_EXACT_STOP, 0);
	    return 0;
	}
	if (!strcmp(the_command_args, "CANON_CONTINUOUS")) {
	    SET_MOTION_CONTROL_MODE(CANON_CONTINUOUS, 0);
	    return 0;
	}
	return INTERP_ERROR;
    }

    if (!strcmp(the_command_name, "MESSAGE")) {
	MESSAGE(the_command_args);
	return 0;
    }

    if (!strcmp(the_command_name, "INIT_CANON")) {
	INIT_CANON();
	return 0;
    }

    if (!strcmp(the_command_name, "TURN_PROBE_OFF")) {
	TURN_PROBE_OFF();
	return 0;
    }

    if (!strcmp(the_command_name, "TURN_PROBE_ON")) {
	TURN_PROBE_ON();
	return 0;
    }

    fprintf(stderr, "canterp: unrecognized canonical command %s\n",
	    the_command);
    return INTERP_ERROR;
}

int Canterp::execute(const char *line, int line_number) {
    return execute(line);
}

int Canterp::execute() {
    return execute(0);
}

int Canterp::open(const char *newfilename) {
    if(f) fclose(f);
    f = fopen(newfilename, "r");
    if(f) snprintf(filename, sizeof(filename), "%s", newfilename);
    return f ? INTERP_OK : INTERP_ERROR;
}

int Canterp::close() {
    return INTERP_OK;
}

int Canterp::exit() { return 0; }
int Canterp::synch() { return 0; }
int Canterp::reset() { return 0; }
int Canterp::line() { return 0; }
int Canterp::call_level() { return 0; }

char *Canterp::line_text(char *buf, size_t bufsize) {
   snprintf(buf, bufsize, "<Canterp::line_text>");
   return buf;
}
char *Canterp::file_name(char *buf, size_t bufsize) {
   snprintf(buf, bufsize, "%s", filename);
   return buf;
}
char *Canterp::file(char *buf, size_t bufsize) {
   snprintf(buf, bufsize, "%s", filename);
   return buf;
}
int Canterp::on_abort(int reason, const char *message)
{
    fprintf(stderr, "Canterp::on_abort reason=%d message='%s'", reason, message);
    reset();
    return INTERP_OK;
}
char *Canterp::command(char *buf, size_t bufsize) {
   snprintf(buf, bufsize, "<Canterp::command>");
   return buf;
}
size_t Canterp::line_length() {
   return 0;
}
int Canterp::sequence_number() {
   return -1;
}
int Canterp::init() { return INTERP_OK; }
void Canterp::active_g_codes(int gees[]) { std::fill(gees, gees + ACTIVE_G_CODES, 0); }
void Canterp::active_m_codes(int emms[]) { std::fill(emms, emms + ACTIVE_M_CODES, 0); }
void Canterp::active_settings(double sets[]) { std::fill(sets, sets + ACTIVE_SETTINGS, 0.0); }
void Canterp::set_loglevel(int level) {}

InterpBase *makeInterp() { return new Canterp; }

