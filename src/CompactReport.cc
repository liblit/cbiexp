#include <sstream>
#include "CompactReport.h"
#include "RunsDirectory.h"

using namespace std;


string CompactReport::suffix;

std::string
CompactReport::format(unsigned runId)
{
  ostringstream collect;
  RunsDirectory::format(collect, runId, "reports.sparse");
  if (!suffix.empty())
    collect << '.' << suffix;
  return collect.str();
}
