#include <cstdio>
#include <iostream>
#include "CompactReport.h"
#include "ReportReader.h"
#include "SiteCoords.h"
#include "fopen.h"
#include "units.h"

using namespace std;


void
ReportReader::read(unsigned runId) const
{
  FILE * const report = fopenRead(CompactReport::format(runId));

  unsigned x, y, z, w;
  bool problem = false;

  while (true)
    {
      SiteCoords coords;
      fscanf(report, "%u\t%u\t", &coords.unitIndex, &coords.siteOffset);
      if (feof(report))
	break;
      if (coords.unitIndex >= num_units)
	{
	  cerr << "unit " << coords.unitIndex << " exceeds unit count (" << num_units << ")\n";
	  problem = true;
	}
      else
	{
	  const unit_t unit = units[coords.unitIndex];
	  if (coords.siteOffset >= unit.num_sites)
	    {
	      cerr << "site " << coords.siteOffset << " exceeds unit's site count (" << unit.num_sites << ")\n";
	      problem = true;
	    }
	  else
	    switch (unit.scheme_code)
	      {
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
		problem = true;
	      }
	}
    }

  fclose(report);

  if (problem)
    exit(1);
}
