#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>           // struct itimerval
#include <sys/ioctl.h>          // ioctl(), TIOCGWINSZ, struct winsize
#include <unistd.h>             // STDIN_FILENO
#include <inttypes.h>

#include "rcs.hh"               // rcs_print_error(), esleep()
#include "emc.hh"               // EMC NML
#include "emc_nml.hh"
#include "emcglb.h"             // EMC_NMLFILE, TRAJ_MAX_VELOCITY
#include "emccfg.h"             // DEFAULT_TRAJ_MAX_VELOCITY
#include "inifile.hh"           // INIFILE
#include "rcs_print.hh"
#include "nml_oi.hh"
#include "timer.hh"

#include <ncurses.h>
#define ESC 27
#define TAB 9
#define RETURN 13
#define ALT(ch) ((ch) + 128)
#define CTL(ch) ((ch) - 64)

static RCS_CMD_CHANNEL *emcCommandBuffer = 0;
static RCS_STAT_CHANNEL *emcStatusBuffer = 0;
EMC_STAT *emcStatus = 0;

// the NML channel for errors
static NML *emcErrorBuffer = 0;
static char error_string[NML_ERROR_LEN] = "";


// NML messages
static EMC_AXIS_ABORT emc_axis_abort_msg;
static EMC_SPINDLE_CONSTANT emc_spindle_constant_msg;

// critical section flag-- set to non-zero to prevent sig handler
// from interrupting your window printing
static unsigned char critFlag = 0;

// the program path prefix
static char programPrefix[LINELEN] = "";
static FILE * programFp = 0;
static int programFpLine = 0;


// error log file
static FILE *errorFp = NULL;
#define ERROR_FILE "keystick.err"
static int xJogPol = 1;
static int yJogPol = 1;
static int zJogPol = 1;

typedef enum {
  AXIS_NONE = 1,
  AXIS_X,
  AXIS_Y,
  AXIS_Z
} AXIS_TYPE;

static AXIS_TYPE axisSelected = AXIS_X;
static AXIS_TYPE axisJogging = AXIS_NONE;

static const char * axisString(AXIS_TYPE a)
{
  switch (a)
    {
    case AXIS_X:
      return "    X SELECTED    ";
    case AXIS_Y:
      return "    Y SELECTED    ";
    case AXIS_Z:
      return "    Z SELECTED    ";
    default:
      return "    ? SELECTED    ";
    }
}

static int axisIndex(AXIS_TYPE a)
{
  if (a == AXIS_X)
    return 0;
  if (a == AXIS_Y)
    return 1;
  if (a == AXIS_Z)
    return 2;

  return 0;
}

typedef enum {
  COORD_RELATIVE = 1,
  COORD_ABSOLUTE
} COORD_TYPE;
static COORD_TYPE coords = COORD_RELATIVE;
typedef enum {
  POS_DISPLAY_ACT = 1,
  POS_DISPLAY_CMD
} POS_DISPLAY_TYPE;
static POS_DISPLAY_TYPE posDisplay = POS_DISPLAY_ACT;

static int spindleChanging = 0;

#define INTERRUPT_USECS 50000
static int usecs = INTERRUPT_USECS;
static chtype ch = 0, oldch = 0;

#define ASCLINELEN 80
static char line_blank[ASCLINELEN + 1];

static char scratch_string[ASCLINELEN] = "";
static char state_string[ASCLINELEN] = "";
static char mode_string[ASCLINELEN] = "";
static char spindle_string[ASCLINELEN] = "";
static char brake_string[ASCLINELEN] = "";
static char mist_string[ASCLINELEN] = "";
static char flood_string[ASCLINELEN] = "";
static char lube_on_string[ASCLINELEN] = "";
static char lube_level_string[ASCLINELEN] = "";
static char home_string[ASCLINELEN] = "";
static char pos_string[ASCLINELEN] = "";
static char origin_string[ASCLINELEN] = "";
static char speed_string[ASCLINELEN] = "";
static char incr_string[ASCLINELEN] = "";
static char prog_string[ASCLINELEN] = "";
static char line_string[ASCLINELEN] = "";
static char interp_string[ASCLINELEN] = "";
static char active_g_codes_string[ASCLINELEN] = "";
static char active_m_codes_string[ASCLINELEN] = "";

// bottom string gill be set to "---Machine Version---"
static char bottom_string[ASCLINELEN + 1] = "";

// key repeat delays, in microseconds
#define DEFAULT_FIRST_KEYUP_DELAY 300000 // works w/ 50000 alarm
static int FIRST_KEYUP_DELAY = DEFAULT_FIRST_KEYUP_DELAY;
#define DEFAULT_NEXT_KEYUP_DELAY  100000 // works w/ 50000 alarm
static int NEXT_KEYUP_DELAY = DEFAULT_NEXT_KEYUP_DELAY;

static int keyup_count = 0;
static enum { DIAG_USECS = 1, DIAG_FIRST_KEYUP_DELAY, DIAG_NEXT_KEYUP_DELAY } 
	diagtab = DIAG_USECS;
static WINDOW * window = 0;
static WINDOW * helpwin = 0;
static WINDOW * diagwin = 0;
static WINDOW * toolwin = 0;
static WINDOW * logwin = 0;
static WINDOW * progwin = 0;

static int wbegy, wbegx;
static int wmaxy, wmaxx;

static void printFkeys()
{
	wattrset(window, A_BOLD);
	mvwaddstr(window, 0, 1, "F1 ");
	mvwaddstr(window, 1, 1, "F2 ");
	mvwaddstr(window, 2, 1, "F3 ");
	mvwaddstr(window, 3, 1, "F4 ");
	mvwaddstr(window, 0, 21, "F5 ");
	mvwaddstr(window, 1, 21, "F6 ");
	mvwaddstr(window, 2, 21, "F7 ");
	mvwaddstr(window, 3, 21, "F8 ");
	mvwaddstr(window, 0, 41, "F9 ");
	mvwaddstr(window, 1, 41, "F10");
	mvwaddstr(window, 2, 41, "F11");
	mvwaddstr(window, 3, 41, "F12");
	mvwaddstr(window, 0, 61, "ESC");
	mvwaddstr(window, 1, 61, "TAB");
	mvwaddstr(window, 2, 61, "END");
	mvwaddstr(window, 3, 61, " ? ");

	wattrset(window, 0);
	mvwaddstr(window, 0, 5, "Estop On/Off  ");
	mvwaddstr(window, 1, 5, "Machine On/Off");
	mvwaddstr(window, 2, 5, "Manual Mode   ");
	mvwaddstr(window, 3, 5, "Auto Mode     ");

	mvwaddstr(window, 0, 25, "MDI Mode      ");
	mvwaddstr(window, 1, 25, "Reset Interp  ");
	mvwaddstr(window, 2, 25, "Mist On/Off   ");
	mvwaddstr(window, 3, 25, "Flood On/Off  ");

	mvwaddstr(window, 0, 45, "Spndl Fwd/Off ");
	mvwaddstr(window, 1, 45, "Spndl Rev/Off ");
	mvwaddstr(window, 2, 45, "Spndl Decrease");
	mvwaddstr(window, 3, 45, "Spndl Increase");

	mvwaddstr(window, 0, 65, "Aborts Actions");
	mvwaddstr(window, 1, 65, "Selects Params");
	mvwaddstr(window, 2, 65, "Quits Display ");
	mvwaddstr(window, 3, 65, "Toggles Help  ");
}

#define ERR_Y ((wmaxy) - 2)
#define ERR_X (wbegx)




static void printError(const char * errstring)
{
	int savey, savex;
	chtype saveattr;

	if (0 == window)
	{
		// no window yet
		return;
	}

	critFlag = 1;

	getyx(window, savey, savex);
	saveattr = getattrs(window);

	mvwaddstr(window, ERR_Y, ERR_X, line_blank);
	wattrset(window, A_BOLD);
	mvwaddstr(window, ERR_Y, ERR_X, errstring);
	(void)wattrset(window, (int)saveattr);
	wmove(window, savey, savex);
	wrefresh(window);

	critFlag = 0;
}

static void printStatus()
{
	int savey, savex;
	int t;
	int line;
	int override;
	int len;
	int code;
	getyx(window, savey, savex);

	if (window == helpwin)
	{
		printFkeys();
		wattrset(window, 0);
		mvwaddstr(window, 5, 1, "x/X y/Y z/Z");
		mvwaddstr(window, 6, 1, "HOME");
		mvwaddstr(window, 7, 1, "< > or , .");
		mvwaddstr(window, 8, 1, "1-9, 0");
		mvwaddstr(window, 9, 1, "<- arrow keys ->");
		mvwaddstr(window, 10, 1, "^ arrow keys V");
		mvwaddstr(window, 11, 1, "PageUp/PageDown");
		mvwaddstr(window, 12, 1, "c/C");
		mvwaddstr(window, 13, 1, "i/I");
		mvwaddstr(window, 14, 1, "#");
		mvwaddstr(window, 15, 1, "@");

		wattrset(window, A_UNDERLINE);
		mvwaddstr(window, 5, 19, "selects axis");
		mvwaddstr(window, 6, 19, "homes selected axis");
		mvwaddstr(window, 7, 19, "change jog speed");
		mvwaddstr(window, 8, 19, "10%-90%, 100% feed");
		mvwaddstr(window, 9, 19, "jog X");
		mvwaddstr(window, 10, 19, "jog Y");
		mvwaddstr(window, 11, 19, "jog Z");
		mvwaddstr(window, 12, 19, "sets continuous jog");
		mvwaddstr(window, 13, 19, "toggles incr jog");
		mvwaddstr(window, 14, 19, "toggles abs/rel");
		mvwaddstr(window, 15, 19, "toggles cmd/act");

		wattrset(window, 0);
		mvwaddstr(window, 5, 41, "B");
		mvwaddstr(window, 6, 41, "b");
		mvwaddstr(window, 7, 41, "o/O");
		mvwaddstr(window, 8, 41, "r/R");
		mvwaddstr(window, 9, 41, "p/P");
		mvwaddstr(window, 10, 41, "s/S");
		mvwaddstr(window, 11, 41, "quote/XYZGM");
		mvwaddstr(window, 12, 41, "l/L");
		mvwaddstr(window, 13, 41, "u/U");

		wattrset(window, A_UNDERLINE);
		mvwaddstr(window, 5, 54, "turns spindle brake on");
		mvwaddstr(window, 6, 54, "turns spindle brake off");
		mvwaddstr(window, 7, 54, "prompts for program");
		mvwaddstr(window, 8, 54, "runs selected program");
		mvwaddstr(window, 9, 54, "pauses motion");
		mvwaddstr(window, 10, 54, "starts motion again");
		mvwaddstr(window, 11, 54, "prompts for MDI command");
		mvwaddstr(window, 12, 54, "prompts for tool table");
		mvwaddstr(window, 13, 54, "turns lube off/on");

		if (error_string[0])
		{
			printError(error_string);
		}

		wattrset(window, A_REVERSE);
		mvwaddstr(window, wmaxy - 1, wbegx, bottom_string);
		wattrset(window, 0);

		// restore cursor position
		wmove(window, savey, savex);
		wrefresh(window);
	}
	else if (window == diagwin)
	{
		wattrset(window, A_BOLD);
		mvwaddstr(window, 0, 34, "Diagnostics");

		wattrset(window, 0);
		mvwaddstr(window, 2, 1, "Task Heartbeat/Cmd:");
		mvwaddstr(window, 3, 1, "IO Heartbeat/Cmd:");
		mvwaddstr(window, 4, 1, "Motion Heartbeat/Cmd:");

		if (diagtab == DIAG_USECS)
			wattrset(window, A_BOLD);
		mvwaddstr(window, 6, 1, "Polling Period (usecs):");
		wattrset(window, 0);
		if (diagtab == DIAG_FIRST_KEYUP_DELAY)
			wattrset(window, A_BOLD);
		mvwaddstr(window, 7, 1, "Kbd Delay Until Repeat:");
		wattrset(window, 0);
		if (diagtab == DIAG_NEXT_KEYUP_DELAY)
			wattrset(window, A_BOLD);
		mvwaddstr(window, 8, 1, "Kbd Delay Between Repeats:");
		wattrset(window, 0);

		mvwaddstr(window, 10, 1, "Task Execution State:");

		mvwaddstr(window, 12, 1, "Traj Scale:");
		mvwaddstr(window, 13, 1, "X Scale:");
		mvwaddstr(window, 14, 1, "Y Scale:");
		mvwaddstr(window, 15, 1, "Z Scale:");

		mvwaddstr(window, 17, 1, "V/Max V:");
		mvwaddstr(window, 18, 1, "A/Max A:");

		wattrset(window, A_UNDERLINE);

		mvwaddstr(window, 4, 28, scratch_string);

		sprintf(scratch_string, "%10d", usecs);
		mvwaddstr(window, 6, 28, scratch_string);
		sprintf(scratch_string, "%10d", FIRST_KEYUP_DELAY);
		mvwaddstr(window, 7, 28, scratch_string);
		sprintf(scratch_string, "%10d", NEXT_KEYUP_DELAY);
		mvwaddstr(window, 8, 28, scratch_string);

		mvwaddstr(window, 10, 28, scratch_string);

		mvwaddstr(window, 12, 28, scratch_string);

		mvwaddstr(window, 17, 28, scratch_string);
		mvwaddstr(window, 18, 28, scratch_string);

		wattrset(window, 0);
		if (error_string[0])
		{
			printError(error_string);
		}

		wattrset(window, A_REVERSE);
		mvwaddstr(window, wmaxy - 1, wbegx, bottom_string);
		wattrset(window, 0);

		// restore cursor position
		wmove(window, savey, savex);
		wrefresh(window);
	}
	else if (window == toolwin)
	{
		wattrset(window, A_BOLD);
		mvwaddstr(window, 0, 34, "Tool Table");

		wattrset(window, 0);
		mvwaddstr(window, 2, 1, "Pocket    ToolNo    Length  Diameter");

		wattrset(window, A_UNDERLINE);
		line = 4;

		wattrset(window, 0);
		if (error_string[0])
		{
			printError(error_string);
		}

		wattrset(window, A_REVERSE);
		mvwaddstr(window, wmaxy - 1, wbegx, bottom_string);
		wattrset(window, 0);

		// restore cursor position
		wmove(window, savey, savex);
		wrefresh(window);
	}
	else if (window == progwin)
	{

	}
	else
	{
		printFkeys();

		wattrset(window, 0);

		mvwaddstr(window, 6, 1, "Override:");
		mvwaddstr(window, 7, 1, "Tool:");
		mvwaddstr(window, 8, 1, "Offset:");

		mvwaddstr(window, 7, 61, "Speed:");
		mvwaddstr(window, 8, 61, "Incr:             ");

		strcpy(scratch_string, "--X--");

		strcpy(scratch_string, "--Y--");
	
		mvwaddstr(window, 10, 47, scratch_string);

		strcpy(scratch_string, "--Z--");
		mvwaddstr(window, 10, 67, scratch_string);

		if (coords == COORD_ABSOLUTE)
		{
			if (posDisplay == POS_DISPLAY_CMD)
			{
				mvwaddstr(window, 11, 1, "Absolute Cmd Pos:");
			}
			else
			{
				mvwaddstr(window, 11, 1, "Absolute Act Pos:");
			}
			mvwaddstr(window, 12, 0, line_blank);
		}
		else
		{
			coords = COORD_RELATIVE;
			if (posDisplay == POS_DISPLAY_CMD)
			{
				mvwaddstr(window, 11, 1, "Relative Cmd Pos:");
			}
			else
			{
				mvwaddstr(window, 11, 1, "Relative Act Pos:");
			}
		}

		mvwaddstr(window, 14, 0, line_blank);
		mvwaddstr(window, 15, 0, line_blank);
		mvwaddstr(window, 16, 0, line_blank);
		mvwaddstr(window, 17, 0, line_blank);
		mvwaddstr(window, 18, 0, line_blank);
		mvwaddstr(window, 19, 0, line_blank);


		wattrset(window, A_REVERSE);
		mvwaddstr(window, wmaxy - 1, wbegx, bottom_string);
		wattrset(window, 0);

		// restore cursor position
		wmove(window, savey, savex);
		wrefresh(window);
	}
}
static int catchErrors = 1;
static int updateStatus()
{
  NMLTYPE type;

  if (0 == emcStatus ||
      0 == emcStatusBuffer ||
      ! emcStatusBuffer->valid())
    {
      return -1;
    }

  if (catchErrors)
    {
      if (0 == emcErrorBuffer ||
          ! emcErrorBuffer->valid())
        {
          return -1;
        }
    }

  switch (type = emcStatusBuffer->peek())
    {
    case -1:
      // error on CMS channel
      return -1;
      break;

    case 0:                     // no new data
    case EMC_STAT_TYPE: // new data
      // new data
      break;

    default:
      return -1;
      break;
    }

  if (catchErrors)
    {
      switch (type = emcErrorBuffer->read())
        {
        case -1:
          // error reading channel
          break;

        case 0:
          // nothing new
          error_string[0] = 0;
          break;

        case EMC_OPERATOR_ERROR_TYPE:
          strncpy(error_string,
                  ((EMC_OPERATOR_ERROR *) (emcErrorBuffer->get_address()))->error,
                  LINELEN - 1);
          error_string[LINELEN - 1] = 0;
          break;

        case EMC_OPERATOR_TEXT_TYPE:
          strncpy(error_string,
                  ((EMC_OPERATOR_TEXT *) (emcErrorBuffer->get_address()))->text,
                  LINELEN - 1);
          error_string[LINELEN - 1] = 0;
          break;

        case EMC_OPERATOR_DISPLAY_TYPE:
          strncpy(error_string,
                  ((EMC_OPERATOR_DISPLAY *) (emcErrorBuffer->get_address()))->display,
                  LINELEN - 1);
          error_string[LINELEN - 1] = 0;
          break;


        case NML_ERROR_TYPE:
          strncpy(error_string,
                  ((NML_ERROR *) (emcErrorBuffer->get_address()))->error,
                  NML_ERROR_LEN - 1);
          error_string[NML_ERROR_LEN - 1] = 0;
          break;

        case NML_TEXT_TYPE:
          strncpy(error_string,
                  ((NML_TEXT *) (emcErrorBuffer->get_address()))->text,
                  NML_ERROR_LEN - 1);
          error_string[NML_ERROR_LEN - 1] = 0;
          break;

        case NML_DISPLAY_TYPE:
          strncpy(error_string,
                  ((NML_DISPLAY *) (emcErrorBuffer->get_address()))->display,
                  NML_ERROR_LEN - 1);
          error_string[NML_ERROR_LEN - 1] = 0;
          break;

        default:
          strcpy(error_string, "unrecognized error");
          break;
        }
    }

  return 0;
}
static int emcCommandSend(RCS_CMD_MSG & cmd) {
}
/*
  startTimer starts the timer to generate SIGALRM events, or stops the timer
  if 'us' is 0. Enable a signal handler for SIGALRM before you call this.
*/
void startTimer(int us)
{
  struct itimerval value;

  value.it_interval.tv_sec = 0;
  value.it_interval.tv_usec = us;
  value.it_value.tv_sec = 0;
  value.it_value.tv_usec = us;

  setitimer(ITIMER_REAL, &value, 0);
}

/*
  alarmHandler is attached to SIGALRM, and handles the reading of NML
  status, the update of the status window, and key-up simulation and
  handling
*/
int sigpipe[2];

static void alarmHandler(int sig, siginfo_t *unused, void *unused2)
{
  char ch = 0;
  write(sigpipe[1], &ch, 1);
}

static void idleHandler()
{
  // read NML status
  updateStatus();

  // only print if main is not printing, so we don't clobber in the middle
  if (! critFlag)
    {
      printStatus();
    }

  // simulate key-up event, as per comment below
  keyup_count -= usecs;
  if (keyup_count < 0)
    {
      keyup_count = 0;
      oldch = 0;
    }

  /*
    Key-up events are simulated as follows: each time a key is received,
    keyup_count is loaded. If it's a new key, FIRST_KEYUP_DELAY is
    loaded. If it's the same as the last key, NEXT_KEYUP_DELAY is loaded.
    This is to handle the different delay between the first and subsequent
    repeats.

    Each time through this handler, keyup_count is decremented. If it
    reaches 0, it means no key has been pressed before the delay expires,
    and we see this as a key-up event.

    If you have code that needs to respond to a key-up event, set a flag
    when you do your key-down stuff, and put the key-up code in here,
    like this:

    if (myFlag && keyup_count == 0)
      {
        // do stuff for key up here

        // and clear your flag
        myFlag = 0;
      }
   */

  // key up for jogs
  if (axisJogging != AXIS_NONE && keyup_count == 0)
    {
      emc_axis_abort_msg.axis = axisIndex(axisJogging);
      emcCommandSend(emc_axis_abort_msg);
      axisJogging = AXIS_NONE;
    }

  // key up for spindle speed changes
  if (spindleChanging && keyup_count == 0)
    {
      emcCommandSend(emc_spindle_constant_msg);
      spindleChanging = 0;
    }

  return;
}

static int done = 0;
static void quit(int sig)
{
  // disable timer
  startTimer(0);

  // clean up curses windows
  delwin(progwin);
  progwin = 0;
  delwin(logwin);
  logwin = 0;
  delwin(toolwin);
  toolwin = 0;
  delwin(diagwin);
  diagwin = 0;
  delwin(helpwin);
  helpwin = 0;
  endwin();		//initscr - endwin 必须成对使用

  // clean up NML buffers

  if (emcErrorBuffer)
    {
      delete emcErrorBuffer;
      emcErrorBuffer = 0;
    }

  if (emcStatusBuffer)
    {
      delete emcStatusBuffer;
      emcStatusBuffer = 0;
      emcStatus = 0;
    }

  if (emcCommandBuffer)
    {
      delete emcCommandBuffer;
      emcCommandBuffer = 0;
    }

  // close program file
  if (programFp)
    {
      fclose(programFp);
      programFp = 0;
    }

  // close error log file
  if (errorFp)
    {
      fclose(errorFp);
      errorFp = NULL;
    }

  // reset signal handlers to default
  signal(SIGALRM, SIG_DFL);
  signal(SIGINT, SIG_DFL);

  exit(0);
}



int ISDEBUG = 0;

// string for ini file version num
static char version_string[LINELEN] = "";

// destructively converts string to its uppercase counterpart
static char *upcase(char *string)
{
  char *ptr = string;

  while (*ptr)
    {
      *ptr = toupper(*ptr);
      ptr++;
    }

  return string;
}

static int iniLoad(const char *filename)
{
  IniFile inifile;
  const char *inistring;
  char machine[LINELEN] = "";
  char version[LINELEN] = "";
  char displayString[LINELEN] = "";
  int jogPol;
  int isdebug = ISDEBUG;
  // open it
  if (!inifile.Open(filename))
    {
	  if(isdebug) printf("cann't load file\n");
      return -1;
    }
 
  
  if ((inistring = inifile.Find("MACHINE", "EMC")))
    {
      strcpy(machine, inistring);

      if ((inistring = inifile.Find("VERSION", "EMC")))
        {
          sscanf(inistring, "$Revision: %s", version);

          sprintf(version_string, "%s EMC Version %s", machine, version);
        }
    }
  if(isdebug) printf("version_string [%s]\n",version_string);
  

  if ((inistring = inifile.Find("MAX_VELOCITY", "TRAJ")))
    {
      if (1 != sscanf(inistring, "%lf", &traj_max_velocity))
        {
          traj_max_velocity = DEFAULT_TRAJ_MAX_VELOCITY;
        }
    }
  else
    {
      traj_max_velocity = DEFAULT_TRAJ_MAX_VELOCITY;
    }
  if(isdebug) printf("traj_max_velocity [%.4f]\n",traj_max_velocity);
	
  if ((inistring = inifile.Find("PROGRAM_PREFIX", "DISPLAY")))
    {
      if (1 != sscanf(inistring, "%s", programPrefix))
        {
          programPrefix[0] = 0;
        }
    }
  else
    {
      programPrefix[0] = 0;
    }
  if(isdebug) printf("programPrefix [%s]\n",programPrefix);
  
  if ((inistring = inifile.Find("POSITION_OFFSET", "DISPLAY")))
    {
      if (1 == sscanf(inistring, "%s", displayString))
        {
          upcase(displayString);
          if (! strcmp(displayString, "ABSOLUTE"))
            {
              coords = COORD_ABSOLUTE;
            }
          else if (! strcmp(displayString, "RELATIVE"))
            {
              coords = COORD_RELATIVE;
            }
          else
            {
              // error-- invalid value
              // ignore
            }
        }
      else
        {
          // error-- no value provided
          // ignore
        }
    }
  else
    {
      // no line at all
      // ignore
    }
  if(isdebug) printf("displayString [%s],coords [%d]\n",displayString,coords);
  
  if ((inistring = inifile.Find("POSITION_FEEDBACK", "DISPLAY")))
    {
      if (1 == sscanf(inistring, "%s", displayString))
        {
          upcase(displayString);
          if (! strcmp(displayString, "ACTUAL"))
            {
              posDisplay = POS_DISPLAY_ACT;
            }
          else if (! strcmp(displayString, "COMMANDED"))
            {
              posDisplay = POS_DISPLAY_CMD;
            }
          else
            {
              // error-- invalid value
              // ignore
            }
        }
      else
        {
          // error-- no value provided
          // ignore
        }
    }
  else
    {
      // no line at all
      // ignore
    }
  if(isdebug) printf("displayString [%s],pos display [%d]\n",displayString,posDisplay);
  
  xJogPol = 1;                  // set to default
  jogPol = 0;
  if ((inistring = inifile.Find("JOGGING_POLARITY", "AXIS_0")) &&
      1 == sscanf(inistring, "%d", &jogPol) &&
      jogPol == 0)
    {
      // it read as 0, so override default
      xJogPol = 0;
    }
  if(isdebug) printf("xJogPol [%d]\n",jogPol);
  
  yJogPol = 1;                  // set to default
  jogPol = 0;
  if ((inistring = inifile.Find("JOGGING_POLARITY", "AXIS_1")) &&
      1 == sscanf(inistring, "%d", &jogPol) &&
      jogPol == 0)
    {
      // it read as 0, so override default
      yJogPol = 0;
    }
  if(isdebug) printf("yJogPol [%d]\n",jogPol);
  
  zJogPol = 1;                  // set to default
  jogPol = 0;
  if ((inistring = inifile.Find("JOGGING_POLARITY", "AXIS_2")) &&
      1 == sscanf(inistring, "%d", &jogPol) &&
      jogPol == 0)
    {
      // it read as 0, so override default
      zJogPol = 0;
    }
  if(isdebug) printf("zJogPol [%d]\n",jogPol);
  // close it
  inifile.Close();

  return 0;;
}

int getch_and_idle()
{
  fd_set readfds;
  while (1)
    {
      FD_ZERO(&readfds);
      FD_SET(0, &readfds);
      FD_SET(sigpipe[0], &readfds);

      select(sigpipe[0] + 1, &readfds, NULL, NULL, NULL);

      if(FD_ISSET(sigpipe[0], &readfds))
        {
          char buf;
          read(sigpipe[0], &buf, 1);
          idleHandler();
          continue;
        }

      if(FD_ISSET(0, &readfds))
        {
          break;
        }
    }

  return getch();
}

int main(int argc, char *argv[])
{
  int dump = 0;
  int usage = 0;
  struct winsize size;
  int curx, cury;
  int t;
  int typing = 0;
#define TYPEBUFFERSIZE ASCLINELEN
  char typebuffer[TYPEBUFFERSIZE];
  char lastmdi[TYPEBUFFERSIZE] = "";
  int typeindex = 0;
  enum {IACT_NONE = 1, IACT_OPEN, IACT_MDI, IACT_LOAD_TOOL, IACT_OPEN_LOG,
        IACT_END} interactive = IACT_NONE;
  //char keystick[] = "keystick";
  int charHandled;
  ISDEBUG = 0;

  // process command line args, indexing argv[] from [1]
  for (t = 1; t < argc; t++)
    {
      // try -dump
      if (!strcmp(argv[t], "-dump"))
        {
          dump = 1;
          t++;          // step over nmlfile
          continue;
        }
	  if(!strcmp(argv[t],"-h") || !strcmp(argv[t],"--help"))
	  {
		  usage = 1;
		  break;
	  }
	  if(!strcmp(argv[t],"--debug"))
	  {
		  ISDEBUG = 1;
		  continue;
	  }
      // try -nml
      if (!strcmp(argv[t], "-nml"))
        {
          if (t == argc - 1)    // if last, can't read past it
            {
              printf("syntax: -nml <nmlfile>\n");
              exit(1);
            }
          else
            {
              strcpy(emc_nmlfile, argv[t+1]);
              t++;              // step over nmlfile
              continue;
            }
        }

      // try -ini
      if (!strcmp(argv[t], "-ini"))
        {
          if (t == argc - 1)
            {
              printf("syntax: -ini <inifile\n");
              exit(1);
            }
          else
            {
              strcpy(emc_inifile, argv[t+1]);
              t++;              // step over inifile
              continue;
            }
        }

      // try -noerror
      if (!strcmp(argv[t], "-noerror"))
        {
          catchErrors = 0;
          continue;
        }

      // try -usecs for cycle time in microseconds
      if (!strcmp(argv[t], "-usecs"))
        {
          if (t == argc - 1 ||
              1 != sscanf(argv[t + 1], "%d", &usecs) ||
              usecs <= 0 ||
              usecs >= 1000000)
            {
              printf("syntax: -usecs <1..999999 microsecond polling period>\n");
              exit(1);
            }
          else
            {
              t++;
              continue;
            }
        }

      // try -dur for delay until repeat
      if (!strcmp(argv[t], "-dur"))
        {
          if (t == argc - 1 ||
              1 != sscanf(argv[t + 1], "%d", &FIRST_KEYUP_DELAY) ||
              FIRST_KEYUP_DELAY < 0)
            {
              printf("syntax: -dur <usecs delay until first repeat>\n");
              exit(1);
            }
          else
            {
              t++;
              continue;
            }
        }

      // try -dbr for delay between repeats
      if (!strcmp(argv[t], "-dbr"))
        {
          if (t == argc - 1 ||
              1 != sscanf(argv[t + 1], "%d", &NEXT_KEYUP_DELAY) ||
              NEXT_KEYUP_DELAY < 0)
            {
              printf("syntax: -dbr <usecs delay between repeats>\n");
              exit(1);
            }
          else
            {
              t++;
              continue;
            }
        }

    }
  if(usage)
  {
	  printf("usage:\n");
	  printf("%s \n",argv[0]);
	  printf("\tuse full path for ini file\n");
	  return 0;
  }
  if (ISDEBUG)  printf("the inifile %s\n",emc_inifile);
  
  iniLoad(emc_inifile);
  // trap SIGINT
  signal(SIGINT, quit);
  pipe(sigpipe);
    // set up handler for SIGALRM to handle periodic timer events
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = alarmHandler;
  sigaction(SIGALRM, &sa, NULL);

  // set up curses
  initscr();
  cbreak();
  noecho();
  nonl();
  intrflush(stdscr, FALSE);
  keypad(stdscr, TRUE);
  helpwin = newwin(0, 0, 0, 0);
  diagwin = newwin(0, 0, 0, 0);
  toolwin = newwin(0, 0, 0, 0);
  logwin = newwin(0, 0, 0, 0);
  progwin = newwin(0, 0, 0, 0);
  window = stdscr;
  
    // fill in strings

  for (t = 0; t < ASCLINELEN; t++) // we all know,it's 80
    {
      line_blank[t] = ' ';
    }
  line_blank[ASCLINELEN] = 0;

  for (t = 0; t < ASCLINELEN; t++)
    {
      bottom_string[t] = '-';
    }
  bottom_string[ASCLINELEN] = 0;
  t = (ASCLINELEN - strlen(version_string)) / 2;
  if (t >= 0)
    {
      memcpy(&bottom_string[t], version_string, strlen(version_string));
    }

  // get screen width and height
  wbegy = 0;
  wbegx = 0;
  if (ioctl(STDIN_FILENO, TIOCGWINSZ, &size) < 0)
    {
      // use 80x24 as default
      wmaxy = 23;
      wmaxx = 79;
    }
  else
    {
      wmaxy = size.ws_row - 1;
      wmaxx = size.ws_col - 1;
    }

  // and set them here
  cury = wmaxy;
  curx = wbegx;
  wmove(window, cury, curx);
  wrefresh(window);
    
  // set up interval timer
  if (!dump)
    {
      startTimer(usecs);
    }
  
  while (! done)
    {
      oldch = ch;
      ch = (chtype) getch_and_idle();

	  // check for ^C that may happen during blocking read
      if (done)
      {
        break;
      }
	  if (ch != oldch)
      {
        keyup_count = FIRST_KEYUP_DELAY;
      }
      else
      {
        keyup_count = NEXT_KEYUP_DELAY;
	  }
	  charHandled = 1;
	  switch (ch)
	  {
	  case  ESC:
		  // task abort
		  break;
	  case  TAB:
		  if (window == diagwin)
		  {
			  
		  }
		  break;
	  case ALT('o'):
	  case  ALT('O'):
		  break;
	  case  CTL('L'):
		  break;;
	  case KEY_F(1):
		  // estop
		  break;
	  case KEY_F(2):
		  // servos
		  break;
	  case KEY_F(3):
		  // manual mode
		  break;
	  case KEY_F(4):
		  // auto mode
		  break;
	  case KEY_F(5):
		  // mdi mode
		  break;;
	  default:
		  charHandled = 0;
		  break;
	  }
	  if (typing && !charHandled)
	  {

	  }
	  switch(ch)
	  {
	  case RETURN:
	    if (oldch != ch)
		{
		  // clear all error buffers
		  critFlag = 1;
		  mvwaddstr(stdscr, ERR_Y, ERR_X, line_blank);
		  mvwaddstr(helpwin, ERR_Y, ERR_X, line_blank);
		  mvwaddstr(diagwin, ERR_Y, ERR_X, line_blank);
		  mvwaddstr(toolwin, ERR_Y, ERR_X, line_blank);
		  mvwaddstr(logwin, ERR_Y, ERR_X, line_blank);
		  mvwaddstr(progwin, ERR_Y, ERR_X, line_blank);
		  wmove(window, wmaxy, wbegx);
		  wrefresh(window);
		  critFlag = 0;
		}
	    break;		
	  }
	}
  quit(0);
  return 0;
}
