#include "CompactReport.h"
#include "RunsDirectory.h"


std::string CompactReport::format(unsigned runId)
{
  return RunsDirectory::format(runId, "reports.sparse");
}
