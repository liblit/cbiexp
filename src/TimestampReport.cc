#include <sstream>
#include "TimestampReport.h"
#include "RunsDirectory.h"

using namespace std;

std::string
TimestampReport::format(unsigned runId, const char *when)
{
  ostringstream collect;
  RunsDirectory::format(collect, runId, "reports.timestamps");
  collect << "." << when;
  return collect.str();
}
