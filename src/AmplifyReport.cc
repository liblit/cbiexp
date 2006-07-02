#include <sstream>
#include "AmplifyReport.h"
#include "RunsDirectory.h"

using namespace std;


string AmplifyReport::suffix;
bool AmplifyReport::amplify;


#ifdef HAVE_ARGP_H

static const argp_option options[] = {
  {
    "report-suffix",
    's',
    "SUFFIX",
    0,
    "add \".SUFFIX\" to amplified report names (default no suffix)",
    0
  },
  {
    "amplify",
    'a',
    0,
    0,
    "uses amplifications when calculating increase scores",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};


static int
parseFlag(int key, char *arg, argp_state *)
{
  using namespace AmplifyReport;

  switch (key)
    {
    case 's':
      AmplifyReport::suffix = arg;
      return 0;
    case 'a':
      AmplifyReport::amplify = true;
      return 0;
    default:
      return ARGP_ERR_UNKNOWN;
    }
}


const argp AmplifyReport::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};

#endif // HAVE_ARGP_H

std::string
AmplifyReport::format(unsigned runId)
{
  ostringstream collect;
  RunsDirectory::format(collect, runId, "amplify");
  if (!suffix.empty())
    collect << '.' << suffix;
  collect << ".log"; 
  return collect.str();
}
