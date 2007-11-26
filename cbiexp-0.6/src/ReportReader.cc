#include <cassert>
#include <cstdio>
#include <fstream>
#include <set>
#include <sstream>
#include <stdexcept>
#include "CompactReport.h"
#include "ReportReader.h"
#include "SiteCoords.h"
#include "StaticSiteInfo.h"
#include "utils.h"

using namespace std;


static set<string> selected;


#ifdef HAVE_ARGP_H

static const argp_option options[] = {
  {
    "scheme",
    's',
    "SCHEME",
    0,
    "only consider SCHEME sites (may be given multiple times; default all schemes)",
    0
  },
  { 0, 0, 0,  0, 0, 0 }
};


static int
parseFlag(int key, char *arg, argp_state *)
{
  switch (key)
    {
    case 's':
      selected.insert(arg);
      return 0;
    default:
      return ARGP_ERR_UNKNOWN;
    }
}


const argp ReportReader::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};

#endif // HAVE_ARGP_H

const string 
ReportReader::format(const unsigned runId) const
{
  return CompactReport::format(runId);
}


bool
ReportReader::selected(const SiteCoords &coords)
{
  if (::selected.empty()) return true;

  static const StaticSiteInfo info;
  const unit_t &unit = info.unit(coords.unitIndex);
  const string &scheme = scheme_name(unit.scheme_code);

  return ::selected.find(scheme) != ::selected.end();
}


void
ReportReader::read(unsigned runId)
{
  const string filename(format(runId));
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

      if (selected(coords))
	{
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
      else
	report.ignore(-1, '\n');
    }
}
