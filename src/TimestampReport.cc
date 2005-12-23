#include <sstream>
#include "TimestampReport.h"
#include "RunsDirectory.h"

using namespace std;

string TimestampReport::when("first");

#ifdef HAVE_ARGP_H

static const argp_option options[] = {
  {
    "timestamp-when",
    'w',
    "WHEN",
    0,
    "which set of timestamps to examine: first entry or last entry (default first)",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};

static int
parseFlag(int key, char *arg, argp_state *)
{
  using namespace TimestampReport;

  switch (key)
    {
    case 'w':
      TimestampReport::when = arg;
      return 0;
    default:
      return ARGP_ERR_UNKNOWN;
    }
}

const argp TimestampReport::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};

#endif // HAVE_ARGP_H

std::string
TimestampReport::format(unsigned runId, const char *w)
{
  ostringstream collect;
  RunsDirectory::format(collect, runId, "reports.timestamps");
  collect << "." << w;
  return collect.str();
}
