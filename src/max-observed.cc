#include <argp.h>
#include <ext/hash_map>
#include <fstream>
#include <iostream>
#include "CompactReport.h"
#include "NumRuns.h"
#include "Progress/Bounded.h"
#include "ReportReader.h"
#include "RunsDirectory.h"
#include "SiteCoords.h"
#include "classify_runs.h"

using namespace std;


typedef __gnu_cxx::hash_map<SiteCoords, unsigned> SitesMap;
SitesMap sites;


////////////////////////////////////////////////////////////////////////
//
//  report reader
//


class Reader : public ReportReader
{
public:
  Reader();
  unsigned increased;

  void branchesSite(    const SiteCoords &, unsigned, unsigned);
  void gObjectUnrefSite(const SiteCoords &, unsigned, unsigned, unsigned, unsigned);
  void returnsSite(     const SiteCoords &, unsigned, unsigned, unsigned);
  void scalarPairsSite( const SiteCoords &, unsigned, unsigned, unsigned);

private:
  void update(const SiteCoords &, unsigned);
};


Reader::Reader()
  : increased(0)
{
}


void
Reader::branchesSite(const SiteCoords &coords, unsigned a, unsigned b)
{
  update(coords, a + b);
}


void
Reader::gObjectUnrefSite(const SiteCoords &coords, unsigned a, unsigned b, unsigned c, unsigned d)
{
  update(coords, a + b + c + d);
}


void
Reader::returnsSite(const SiteCoords &coords, unsigned a, unsigned b, unsigned c)
{
  update(coords, a + b + c);
}


void
Reader::scalarPairsSite(const SiteCoords &coords, unsigned a, unsigned b, unsigned c)
{
  update(coords, a + b + c);
}


void
Reader::update(const SiteCoords &coords, unsigned observations)
{
  unsigned &maxObserved = sites[coords];
  if (maxObserved < observations)
    {
      maxObserved = observations;
      ++increased;
    }
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
    ofstream increases("max-observed-increases.txt");

    for (unsigned runId = 0; runId < numRuns; runId++)
      {
	progress.step();
	if (is_srun[runId] || is_frun[runId])
	  {
	    Reader reader;
	    reader.read(runId);
	    increases << reader.increased << '\n';
	  }
      }
  }

  for (SitesMap::const_iterator site = sites.begin(); site != sites.end(); ++site)
    cout << site->first << '\t' << site->second << '\n';

  return 0;
}
