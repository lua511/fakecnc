#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>          // ioctl(), TIOCGWINSZ, struct winsize

#include "emcglb.h"
#include "emccfg.h"             // DEFAULT_TRAJ_MAX_VELOCITY
#include "inifile.hh"           // INIFILE

#define INTERRUPT_USECS 50000
static int usecs = INTERRUPT_USECS;

#define ASCLINELEN 80
// key repeat delays, in microseconds
#define DEFAULT_FIRST_KEYUP_DELAY 300000 // works w/ 50000 alarm
static int FIRST_KEYUP_DELAY = DEFAULT_FIRST_KEYUP_DELAY;
#define DEFAULT_NEXT_KEYUP_DELAY  100000 // works w/ 50000 alarm
static int NEXT_KEYUP_DELAY = DEFAULT_NEXT_KEYUP_DELAY;

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

static char programPrefix[LINELEN] = "";

static int xJogPol = 1;
static int yJogPol = 1;
static int zJogPol = 1;

static int catchErrors = 1;

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

  // open it
  if (!inifile.Open(filename))
    {
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

  xJogPol = 1;                  // set to default
  if ((inistring = inifile.Find("JOGGING_POLARITY", "AXIS_0")) &&
      1 == sscanf(inistring, "%d", &jogPol) &&
      jogPol == 0)
    {
      // it read as 0, so override default
      xJogPol = 0;
    }

  yJogPol = 1;                  // set to default
  if ((inistring = inifile.Find("JOGGING_POLARITY", "AXIS_1")) &&
      1 == sscanf(inistring, "%d", &jogPol) &&
      jogPol == 0)
    {
      // it read as 0, so override default
      yJogPol = 0;
    }

  zJogPol = 1;                  // set to default
  if ((inistring = inifile.Find("JOGGING_POLARITY", "AXIS_2")) &&
      1 == sscanf(inistring, "%d", &jogPol) &&
      jogPol == 0)
    {
      // it read as 0, so override default
      zJogPol = 0;
    }

  // close it
  inifile.Close();

  return 0;;
}

int main(int argc, char *argv[])
{
  int dump = 0;
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
  iniLoad(emc_inifile);
	printf("hello\n");
	return 0;
}
