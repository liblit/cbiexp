#include <sstream>
#include "RunsDirectory.h"

using namespace std;


const char *RunsDirectory::root = ".";


static const argp_option options[] = {
  {
    "runs-directory",
    'r',
    "DIR",
    0,
    "look for scripted runs in DIR (default \".\")",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};


static int
parseFlag(int key, char *arg, argp_state *)
{
  switch (key) {
  case 'r':
    RunsDirectory::root = arg;
    return 0;
  default:
    return ARGP_ERR_UNKNOWN;
  }
}


const argp RunsDirectory::argp = {
    options, parseFlag, 0, 0, 0, 0, 0
};


string
RunsDirectory::format(unsigned runId, const char filename[])
{
  ostringstream collect;
  collect << root << '/' << runId << '/' << filename;
  return collect.str();
}
