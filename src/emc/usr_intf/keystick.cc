#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>          // ioctl(), TIOCGWINSZ, struct winsize
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>             // STDIN_FILENO
#include <ncurses.h>

#include "emcglb.h"
#include "emccfg.h"             // DEFAULT_TRAJ_MAX_VELOCITY
#include "inifile.hh"           // INIFILE

#define INTERRUPT_USECS 50000
static int usecs = INTERRUPT_USECS;
static chtype ch = 0, oldch = 0;

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

static WINDOW * window = 0;
static WINDOW * helpwin = 0;
static WINDOW * diagwin = 0;
static WINDOW * toolwin = 0;
static WINDOW * logwin = 0;
static WINDOW * progwin = 0;

static int wbegy, wbegx;
static int wmaxy, wmaxx;
#define ASCLINELEN 80
static char line_blank[ASCLINELEN + 1];
// bottom string gill be set to "---Machine Version---"
static char bottom_string[ASCLINELEN + 1] = "";

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
static void idleHandler()
{
	
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

int sigpipe[2];
static void alarmHandler(int sig, siginfo_t *unused, void *unused2)
{
  char ch = 0;
  write(sigpipe[1], &ch, 1);
}

static int done = 0;

static void quit(int sig)
{
  exit(0);
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

int ISDEBUG = 0;
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
  }
  printf("the inifile %s\n",emc_inifile);
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

  for (t = 0; t < ASCLINELEN; t++)
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
	}
		
  return 0;
}
