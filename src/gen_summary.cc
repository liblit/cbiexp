#include <argp.h>
#include <cstdlib>
#include <iostream>
#include <map>
#include "CompactReport.h"
#include "Confidence.h"
#include "NumRuns.h"
#include "PredStats.h"
#include "RunsDirectory.h"
#include "SourceDirectory.h"
#include "StaticSiteInfo.h"
#include "classify_runs.h"
#include "fopen.h"
#include "utils.h"

using namespace std;


typedef map<char, unsigned> Tally;


StaticSiteInfo staticSiteInfo;


////////////////////////////////////////////////////////////////////////
//
//  Command line processing
//


static void
processCommandLine(int argc, char *argv[])
{
  static const argp_child children[] = {
    { &CompactReport::argp, 0, 0, 0 },
    { &Confidence::argp, 0, 0, 0 },
    { &NumRuns::argp, 0, 0, 0 },
    { &RunsDirectory::argp, 0, 0, 0 },
    { &SourceDirectory::argp, 0, 0, 0 },
    { 0, 0, 0, 0 }
  };

  static const argp argp = {
    0, 0, 0, 0, children, 0, 0
  };

  argp_parse(&argp, argc, argv, 0, 0, 0);
} 



////////////////////////////////////////////////////////////////////////
//
//  XML printing functions
//


static void
print_summary_scheme(ostream &out, const char name[], Tally &total, Tally &retain, char key)
{
  out << "<scheme name=\"" << name
      << "\" total=\"" << total[key]
      << "\" retain=\"" << retain[key]
      << "\"/>";
}


static void
print_summary(ostream &out, Tally &tally)
{
  time_t now;
  time(&now);
  char almost[sizeof("yyyy-mm-ddThh:mm:ss+hhmm")];
  const size_t converted = strftime(almost, sizeof(almost), "%FT%T%z", localtime(&now));
  assert(converted > 0);
  string timestamp(almost);
  timestamp.insert(22, ":");

  out << "<?xml version=\"1.0\"?>"
      << "<?xml-stylesheet type=\"text/xsl\" href=\"summary.xsl\"?>"
      << "<!DOCTYPE experiment SYSTEM \"summary.dtd\">"

      << "<experiment date=\"" << timestamp
      << "\" source-dir=\"" << SourceDirectory::root
      << "\">"

      << "<runs success=\"" << num_sruns
      << "\" failure=\"" << num_fruns
      << "\" ignore=\"" << NumRuns::count() - (num_sruns + num_fruns)
      << "\"/>"

      << "<analysis confidence=\"" << Confidence::level << "\"/>"

      << "<schemes>";

  Tally total;
  for (StaticSiteInfo::SiteIterator site = staticSiteInfo.sitesBegin();
       site != staticSiteInfo.sitesEnd(); ++site)
    {
      unsigned predsPerSite;
      switch (site->scheme_code)
	{
	case 'B':
	  predsPerSite = 2;
	  break;
	case 'R':
	case 'S':
	  predsPerSite = 6;
	  break;
	case 'G':
	  predsPerSite = 8;
	  break;
	case 'F':
	  predsPerSite = 18;
	  break;
	default:
	  cerr << "unknown instrumentation scheme code '"
	       << site->scheme_code << "'\n";
	  exit(1);
	}

      total[site->scheme_code] += predsPerSite;
    }

  print_summary_scheme(out, "branches", total, tally, 'B');
  print_summary_scheme(out, "float-kinds", total, tally, 'F');
  print_summary_scheme(out, "g-object-unref", total, tally, 'G');
  print_summary_scheme(out, "returns", total, tally, 'R');
  print_summary_scheme(out, "scalar-pairs", total, tally, 'S');

  out << "</schemes></experiment>\n";
}


////////////////////////////////////////////////////////////////////////
//
//  main
//


int
main(int argc, char** argv)
{
  processCommandLine(argc, argv);
  classify_runs();
  ios::sync_with_stdio(false);

  Tally tally;
  FILE *predStats = fopenRead(PredStats::filename);

  pred_info info;
  while (read_pred_full(predStats, info))
    ++tally[staticSiteInfo.site(info.siteIndex).scheme_code];

  fclose(predStats);

  print_summary(cout, tally);

  return 0;
}
