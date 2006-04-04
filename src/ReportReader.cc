#include <cassert>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "CompactReport.h"
#include "ReportReader.h"
#include "SiteCoords.h"
#include "fopen.h"

using namespace std;


void
ReportReader::read(unsigned runId)
{
  const string filename(CompactReport::format(runId));
  ifstream report;
  report.exceptions(ios::badbit);
  report.open(filename.c_str());

  if (!report)
    {
      const int code = errno;
      ostringstream message;
      message << "cannot read " << filename << ": " << strerror(code);
      throw runtime_error(message.str());
    }

  while (true)
    {
      SiteCoords coords;
      report >> coords.unitIndex >> coords.siteOffset;
      if (report.eof())
	break;

      vector<count_tp> counts;
      counts.reserve(3);

      while (report.peek() != '\n')
	{
	  count_tp count;
	  report >> count;
	  counts.push_back(count);
	}

      handleSite(coords, counts);
    }
}
