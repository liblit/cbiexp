#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sysexits.h>
#include "NumRuns.h"
#include "RunsDirectory.h"

using namespace std;

unsigned NumRuns::begin = 0;
unsigned NumRuns::end = 0;


static const argp_option options[] = {
  {
    "begin-runs",
    'b',
    "NUM",
    0,
    "begin counting runs from NUM (default 0)",
    0,
  },
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
    case 'b':
      begin = parseNum(arg, state);
      return 0;

    case 'e':
      end = parseNum(arg, state);
      if (end == 0)
	invalid(arg, state);
      return 0;

    case ARGP_KEY_END:
      if (end == 0)
	{
	  string stampName(RunsDirectory::root);
	  stampName += "/stamp-labels";

	  ifstream stampFile(stampName.c_str());
	  stampFile >> end;
	  if (!stampFile)
	    {
	      cerr << "cannot read run count from " << stampName << '\n';
	      exit(1);
	    }
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
