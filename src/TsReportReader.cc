#include <cassert>
#include <cstdio>
#include <iostream>
#include "TsReportReader.h"
#include "SiteCoords.h"
#include "fopen.h"
#include "units.h"

using namespace std;

void TsReportReader::read(unsigned runId, const char *when)
{
  FILE * const report = fopenRead(TimestampReport::format(runId, when));

  SiteCoords coords;
  unsigned ts;

  while (true)
    {
      fscanf(report, "%u\t%u\t%u\n", &coords.unitIndex, &coords.siteOffset, &ts);
      if (feof(report))
        break;

      assert(coords.unitIndex < num_units);
      const unit_t unit = units[coords.unitIndex];
      assert(coords.siteOffset < unit.num_sites);

      siteTs(coords, ts);
    }

  fclose(report);
}
