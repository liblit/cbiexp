#include <cstdio>
#include <iostream>
#include "CompactReport.h"
#include "ReportReader.h"
#include "fopen.h"
#include "units.h"

using namespace std;


void
ReportReader::read(unsigned runId) const
{
  FILE * const report = fopenRead(CompactReport::format(runId));

  unsigned u, c, x, y, z, w;
  bool problem = false;

  while (true)
    {
      fscanf(report, "%u\t%u\t", &u, &c);
      if (feof(report))
	break;
      if (u >= num_units)
	{
	  cerr << "unit " << u << " exceeds unit count (" << num_units << ")\n";
	  problem = true;
	}
      else
	{
	  const unit_t unit = units[u];
	  if (c >= unit.num_sites)
	    {
	      cerr << "site " << c << " exceeds unit's site count (" << unit.num_sites << ")\n";
	      problem = true;
	    }
	  else
	    switch (unit.scheme_code)
	      {
	      case 'B':
		fscanf(report, "%u\t%u\n", &x, &y);
		branchesSite(u, c, x, y);
		break;
	      case 'G':
		fscanf(report, "%u\t%u\t%u\t%u\n", &x, &y, &z, &w);
		gObjectUnrefSite(u, c, x, y, z, w);
		break;
	      case 'R':
		fscanf(report, "%u\t%u\t%u\n", &x, &y, &z);
		returnsSite(u, c, x, y, z);
		break;
	      case 'S':
		fscanf(report, "%u\t%u\t%u\n", &x, &y, &z);
		scalarPairsSite(u, c, x, y, z);
		break;
	      default:
		cerr << "unit " << u << " has unknown scheme code '" << unit.scheme_code << "'\n";
		problem = true;
	      }
	}
    }

  fclose(report);

  if (problem)
    exit(1);
}
