#include <sstream>
#include "CompactReport.h"
#include "RunsDirectory.h"

using namespace std;


string CompactReport::suffix;


static const argp_option options[] = {
  {
    "report-suffix",
    's',
    "SUFFIX",
    0,
    "append \".SUFFIX\" to converted report names (default no suffix)",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};


static int
parseFlag(int key, char *arg, argp_state *)
{
  using namespace CompactReport;

  switch (key)
    {
    case 's':
      CompactReport::suffix = arg;
      return 0;
    default:
      return ARGP_ERR_UNKNOWN;
    }
}


const argp CompactReport::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};


std::string
CompactReport::format(unsigned runId)
{
  ostringstream collect;
  RunsDirectory::format(collect, runId, "reports.sparse");
  if (!suffix.empty())
    collect << '.' << suffix;
  return collect.str();
}
