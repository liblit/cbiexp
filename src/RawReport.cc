#include "RawReport.h"
#include "RunsDirectory.h"


std::string RawReport::format(unsigned runId)
{
  return RunsDirectory::format(runId, "reports");
}
