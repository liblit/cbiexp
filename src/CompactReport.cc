#include <sstream>
#include "CompactReport.h"
#include "RunsDirectory.h"

using namespace std;


unsigned CompactReport::sparsity = 1;


static const argp_option options[] = {
  {
    "sparsity",
    's',
    "NUM",
    0,
    "use reports downsampled at rate 1/NUM (default 1)",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};


static int
parseFlag(int key, char *arg, argp_state *state)
{
  using namespace CompactReport;

  switch (key)
    {
    case 's':
      char *tail;
      errno = 0;
      sparsity = strtoul(arg, &tail, 0);
      if (errno || *tail || sparsity == 0)
	argp_error(state, "invalid sparsity \"%s\"", arg);
      return 0;
    default:
      return ARGP_ERR_UNKNOWN;
    }
}


const argp CompactReport::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};


std::string
CompactReport::format(unsigned runId, unsigned sparsity)
{
  ostringstream collect;
  RunsDirectory::format(collect, runId, "reports.sparse");
  if (sparsity != 1)
    collect << '.' << sparsity;
  return collect.str();
}
