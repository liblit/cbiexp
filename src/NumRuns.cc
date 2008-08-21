#include <cstdlib>
#include <fstream>
#include <iostream>
#include "NumRuns.h"
#include "RunsDirectory.h"

using namespace std;

unsigned NumRuns::begin_val = 0;
unsigned NumRuns::end_val = 0;

bool NumRuns::end_is_set = false;
bool NumRuns::begin_is_set = false;

#ifdef HAVE_ARGP_H
#include <sysexits.h>

static const argp_option options[] = {
  {
    "begin-runs",
    'b',
    "NUM",
    0,
    "begin counting runs at NUM",
    0,
  },
  {
    "end-runs",
    'e',
    "NUM",
    0,
    "end counting runs at NUM-1",
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

static unsigned
parseNumAndSetFlag(const char *arg, argp_state *state, bool* flag) 
{
  unsigned result = parseNum(arg, state);   
  *flag = true;
  return result;
}


static int
parseFlag(int key, char *arg, argp_state *state)
{
  using namespace NumRuns;

  switch (key)
    {
    case 'b':
      begin_val = parseNumAndSetFlag(arg, state, &begin_is_set);
      return 0;

    case 'e':
      end_val = parseNumAndSetFlag(arg, state, &end_is_set);
      if (end_val == 0)
        invalid(arg, state);
      return 0;

    case ARGP_KEY_END:
      // make sure that both values are set on command line 
      if (!end_is_set || !begin_is_set)
        argp_error(state, "must set begin-runs and end-runs");

      if (begin_val >= end_val)
	argp_failure(state, EX_USAGE, 0, "no runs to read (begin %d >= end %d)", begin_val, end_val);

    default:
      return ARGP_ERR_UNKNOWN;
    }
}


const argp NumRuns::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};

#endif // HAVE_ARGP_H
