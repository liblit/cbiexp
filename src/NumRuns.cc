#include <cstdlib>
#include <fstream>
#include <iostream>
#include "NumRuns.h"
#include "RunsDirectory.h"

using namespace std;

static unsigned explicitValue = 0;


#define LONG_FLAG "number-of-runs"


static const argp_option options[] = {
  {
    LONG_FLAG,
    'n',
    "COUNT",
    0,
    "process COUNT runs starting from 0; default is read from \"stamp-labels\" in runs directory",
    0,
  },
  { 0, 0, 0, 0, 0, 0 }
};


static int parseFlag(int key, char *arg, argp_state *state)
{
  switch (key)
    {
    case 'n':
      char *tail;
      errno = 0;
      explicitValue = strtoul(arg, &tail, 0);
      if (errno || *tail || !explicitValue)
	argp_error(state, "invalid run count \"%s\"", arg);
      return 0;
    default:
      return ARGP_ERR_UNKNOWN;
    }
}


const argp NumRuns::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};


unsigned
NumRuns::value()
{
  if (!explicitValue)
    {
      string stampName(RunsDirectory::root);
      stampName += "/stamp-labels";

      ifstream stampFile(stampName.c_str());
      stampFile >> explicitValue;
      if (!stampFile)
	{
	  cerr << "cannot read run count from " << stampName << '\n';
	  exit(1);
	}

      if (!explicitValue)
	{
	  cerr << "no runs to process\n";
	  exit(1);
	}
    }

  return explicitValue;
}
