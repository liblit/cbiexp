#include <cassert>
#include <cstdio>
#include <fstream>
#include "CompactReport.h"
#include "ReportReader.h"
#include "SiteCoords.h"
#include "fopen.h"

using namespace std;


void
ReportReader::read(unsigned runId)
{
  ifstream report;
  report.exceptions(ios::badbit);
  report.open(CompactReport::format(runId).c_str());

  while (true)
    {
      SiteCoords coords;
      report >> coords.unitIndex >> coords.siteOffset;
      if (report.eof())
	break;

      vector<unsigned> counts;
      counts.reserve(3);

      while (report.peek() != '\n')
	{
	  unsigned count;
	  report >> count;
	  counts.push_back(count);
	}

      handleSite(coords, counts);
    }
}
