#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>          // ioctl(), TIOCGWINSZ, struct winsize

#include "emcglb.h"

#define INTERRUPT_USECS 50000
static int usecs = INTERRUPT_USECS;

#define ASCLINELEN 80
// key repeat delays, in microseconds
#define DEFAULT_FIRST_KEYUP_DELAY 300000 // works w/ 50000 alarm
static int FIRST_KEYUP_DELAY = DEFAULT_FIRST_KEYUP_DELAY;
#define DEFAULT_NEXT_KEYUP_DELAY  100000 // works w/ 50000 alarm
static int NEXT_KEYUP_DELAY = DEFAULT_NEXT_KEYUP_DELAY;

static int catchErrors = 1;

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
	
	printf("hello\n");
	return 0;
}
