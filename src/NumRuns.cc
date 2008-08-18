#include <cstdlib>
#include <fstream>
#include <iostream>
#include "NumRuns.h"
#include "RunsDirectory.h"

using namespace std;

unsigned NumRuns::begin = 0;
unsigned NumRuns::end = 0;

#ifdef HAVE_ARGP_H
#include <sysexits.h>

static const argp_option options[] = {
  {
    "end-runs",
    'e',
    "NUM",
    0,
    "end counting runs at NUM-1; default uses entire dataset",
    0,
  },
  { 0, 0, 0, 0, 0, 0 }
};


static void
invalid(const char *arg, argp_state *state)
{
  argp_error(state, "invalid run count \"%s\"", arg);
}


static unsigned
parseNum(const char *arg, argp_state *state)
{
  errno = 0;
  char *tail;
  const unsigned converted = strtoul(arg, &tail, 0);
  if (errno || *tail)
    invalid(arg, state);
  return converted;
}


static int
parseFlag(int key, char *arg, argp_state *state)
{
  using namespace NumRuns;

  switch (key)
    {
    case 'e':
      end = parseNum(arg, state);
      if (end == 0)
	invalid(arg, state);
      return 0;

    case ARGP_KEY_END:
      if (end == 0)
	{
	  ifstream runsfile("runs.txt");
          char buf[1024];
          int count = 0;
          while(runsfile.peek() != EOF) {
            // read a line until done
            do {
              runsfile.get(buf, 1024);
            } while (runsfile.gcount() != 0);

            // throw out end of line
            if (runsfile.peek() != EOF)
                runsfile.ignore(1);
            count++; 
          }
          end = count;
	}

      if (begin >= end)
	argp_failure(state, EX_USAGE, 0, "no runs to read (begin %d >= end %d)", begin, end);

    default:
      return ARGP_ERR_UNKNOWN;
    }
}


const argp NumRuns::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};

#endif // HAVE_ARGP_H
