#include <cassert>
#include <cstdio>
#include <iostream>
#include "CompactReport.h"
#include "ReportReader.h"
#include "SiteCoords.h"
#include "Units.h"
#include "fopen.h"

using namespace std;


void
ReportReader::read(unsigned runId)
{
  Units units;
  FILE * const report = fopenRead(CompactReport::format(runId));

  SiteCoords coords;

  while (true)
    {
      fscanf(report, "%u\t%u\t", &coords.unitIndex, &coords.siteOffset);
      if (feof(report))
	break;

      assert(coords.unitIndex < units.size);
      const unit_t &unit = units[coords.unitIndex];
      assert(coords.siteOffset < unit.num_sites);

      switch (unit.scheme_code)
	{
	  unsigned x, y, z, w;
	case 'B':
	  fscanf(report, "%u\t%u\n", &x, &y);
	  branchesSite(coords, x, y);
	  break;
	case 'G':
	  fscanf(report, "%u\t%u\t%u\t%u\n", &x, &y, &z, &w);
	  gObjectUnrefSite(coords, x, y, z, w);
	  break;
	case 'R':
	  fscanf(report, "%u\t%u\t%u\n", &x, &y, &z);
	  returnsSite(coords, x, y, z);
	  break;
	case 'S':
	  fscanf(report, "%u\t%u\t%u\n", &x, &y, &z);
	  scalarPairsSite(coords, x, y, z);
	  break;
	default:
	  cerr << "unit " << coords.unitIndex << " has unknown scheme code '" << unit.scheme_code << "'\n";
	  exit(1);
	}
    }

  fclose(report);
}
