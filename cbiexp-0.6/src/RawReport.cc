#include "RawReport.h"
#include "RunsDirectory.h"

using namespace std;


const char *name = "reports";


#if HAVE_ARGP_H

static const argp_option options[] = {
  {
    "raw-report-name",
    'r',
    "FILE",
    0,
    "look for raw reports named FILE (default \"reports\")",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};


static int
parseFlag(int key, char *arg, argp_state *)
{
  switch (key) {
  case 'r':
    name = arg;
    return 0;
  default:
    return ARGP_ERR_UNKNOWN;
  }
}


const argp RawReport::argp = {
    options, parseFlag, 0, 0, 0, 0, 0
};

#endif // HAVE_ARGP_H




string RawReport::format(unsigned runId)
{
  return RunsDirectory::format(runId, name);
}
