#include <argp.h>
#include <ext/hash_map>
#include <iostream>
#include "CompactReport.h"
#include "NumRuns.h"
#include "Progress/Bounded.h"
#include "ReportReader.h"
#include "RunsDirectory.h"
#include "SiteCoords.h"
#include "classify_runs.h"

using namespace std;


enum Classification { Rare, Common };
typedef __gnu_cxx::hash_map<SiteCoords, Classification> SitesMap;
SitesMap sites;


////////////////////////////////////////////////////////////////////////
//
//  report reader
//


class Reader : public ReportReader
{
public:
  void branchesSite(    const SiteCoords &, unsigned, unsigned) const;
  void gObjectUnrefSite(const SiteCoords &, unsigned, unsigned, unsigned, unsigned) const;
  void returnsSite(     const SiteCoords &, unsigned, unsigned, unsigned) const;
  void scalarPairsSite( const SiteCoords &, unsigned, unsigned, unsigned) const;

private:
  void classify(const SiteCoords &, unsigned) const;
};


void
Reader::branchesSite(const SiteCoords &coords, unsigned a, unsigned b) const
{
  classify(coords, a + b);
}


void
Reader::gObjectUnrefSite(const SiteCoords &coords, unsigned a, unsigned b, unsigned c, unsigned d) const
{
  classify(coords, a + b + c + d);
}


void
Reader::returnsSite(const SiteCoords &coords, unsigned a, unsigned b, unsigned c) const
{
  classify(coords, a + b + c);
}


void
Reader::scalarPairsSite(const SiteCoords &coords, unsigned a, unsigned b, unsigned c) const
{
  classify(coords, a + b + c);
}


void
Reader::classify(const SiteCoords &coords, unsigned observations) const
{
  Classification &classification = sites[coords];
  if (classification == Rare && observations > 10)
    classification = Common;
}


////////////////////////////////////////////////////////////////////////
//
//  Command line processing
//


static void
processCommandLine(int argc, char **argv)
{
  static const argp_child children[] = {
    { &CompactReport::argp, 0, 0, 0 },
    { &NumRuns::argp, 0, 0, 0 },
    { &RunsDirectory::argp, 0, 0, 0 },
    { 0, 0, 0, 0 }
  };

  static const argp argp = {
    0, 0, 0, 0, children, 0, 0
  };

  argp_parse(&argp, argc, argv, 0, 0, 0);
};


////////////////////////////////////////////////////////////////////////
//
//  The main event
//


int
main(int argc, char** argv)
{
  set_terminate(__gnu_cxx::__verbose_terminate_handler);
  processCommandLine(argc, argv);

  {
    classify_runs();
    const unsigned numRuns = NumRuns::value();
    Progress::Bounded progress("scanning reports", numRuns);
    Reader reader;

    for (unsigned runId = 0; runId < numRuns; runId++)
      {
	progress.step();
	if (is_srun[runId] || is_frun[runId])
	  reader.read(runId);
      }
  }

  {
    Progress::Bounded progress("printing rare site list", sites.size());
    for (SitesMap::const_iterator site = sites.begin(); site != sites.end(); ++site)
      {
	progress.step();
	if (site->second == Rare)
	  cout << site->first << '\n';
      }
  }

  return 0;
}
