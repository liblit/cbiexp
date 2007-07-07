#include <sstream>
#include "CompactReport.h"
#include "AmplifyReport.h"
#include "RunsDirectory.h"

using namespace std;


bool AmplifyReport::amplify;


#ifdef HAVE_ARGP_H

static const argp_option options[] = {
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
parseFlag(int key, char *, argp_state *)
{
  using namespace AmplifyReport;

  switch (key)
    {
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
  string suffix = CompactReport::suffix;
  if (!suffix.empty())
    collect << '.' << suffix;
  collect << ".log"; 
  return collect.str();
}
