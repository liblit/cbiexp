#include <argp.h>
#include <iostream>
#include <map>
#include "CompactReport.h"
#include "Confidence.h"
#include "NumRuns.h"
#include "PredStats.h"
#include "RunsDirectory.h"
#include "SourceDirectory.h"
#include "classify_runs.h"
#include "fopen.h"
#include "sites.h"
#include "units.h"
#include "utils.h"

using namespace std;


typedef map<char, unsigned> Tally;


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
print_summary_scheme(ostream &out, const char name[], size_t total, size_t retain)
{
  out << "<scheme name=\"" << name
      << "\" total=\"" << total
      << "\" retain=\"" << retain
      << "\"/>";
}


static void
print_summary(ostream &out, Tally &tally)
{
  time_t now;
  time(&now);

  out << "<?xml version=\"1.0\"?>"
      << "<?xml-stylesheet type=\"text/xsl\" href=\"summary.xsl\"?>"
      << "<!DOCTYPE experiment SYSTEM \"summary.dtd\">"

      << "<experiment date=\"" << ctime(&now)
      << "\" source-dir=\"" << SourceDirectory::root
      << "\">"

      << "<runs success=\"" << num_sruns
      << "\" failure=\"" << num_fruns
      << "\" ignore=\"" << NumRuns::value() - (num_sruns + num_fruns)
      << "\"/>"

      << "<analysis confidence=\"" << Confidence::level << "\"/>"

      << "<schemes>";

  print_summary_scheme(out, "branches", NumBPreds, tally['B']);
  print_summary_scheme(out, "returns", NumRPreds, tally['R']);
  print_summary_scheme(out, "scalar-pairs", NumSPreds, tally['S']);
  print_summary_scheme(out, "g-object-unref", NumGPreds, tally['G']);

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
  {
    FILE *predStats = fopenRead(PredStats::filename);

    pred_info info;
    while (read_pred_full(predStats, info))
      ++tally[sites[info.siteIndex].scheme_code];

    fclose(predStats);
  }

  print_summary(cout, tally);

  return 0;
}
