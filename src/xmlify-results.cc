#include <argp.h>
#include <cmath>
#include <ext/hash_set>
#include <fstream>
#include <list>
#include <sysexits.h>
#include "PredStats.h"
#include "SiteCoords.h"
#include "fopen.h"
#include "sites.h"
#include "units.h"
#include "utils.h"

using namespace std;


static __gnu_cxx::hash_set<SiteCoords> rareSites;
static list<string> prefixes;


////////////////////////////////////////////////////////////////////////
//
//  command line processing
//


static int
parseFlag(int key, char *arg, argp_state *state)
{
  switch (key)
    {
    case 'r':
      {
	ifstream file(arg);
	if (!file)
	  argp_failure(state, EX_NOINPUT, errno, "cannot read rare site list %s", arg);
	file.exceptions(ios::badbit);
	copy(istream_iterator<SiteCoords>(file), istream_iterator<SiteCoords>(), inserter(rareSites, rareSites.end()));
	return 0;
      }

    case 'p':
      prefixes.push_back(arg);
      return 0;

    default:
      return ARGP_ERR_UNKNOWN;
    }
}


static void
processCommandLine(int argc, char *argv[])
{
  static const argp_option options[] = {
    { "rare-sites", 'r', "FILE", 0, "mark sites listed in FILE as rare", 0 },
    { "strip-prefix", 'p', "TEXT", 0, "remove TEXT from the start of file names; may be given multiple times", 0 },
    { 0, 0, 0, 0, 0, 0 }
  };

  static const argp argp = {
    options, parseFlag, 0, 0, 0, 0, 0
  };

  argp_parse(&argp, argc, argv, 0, 0, 0);
}


////////////////////////////////////////////////////////////////////////
//
//  The main event
//


int main(int argc, char *argv[])
{
  set_terminate(__gnu_cxx::__verbose_terminate_handler);
  processCommandLine(argc, argv);
  ios::sync_with_stdio(false);

  ofstream xml("predictor-info.xml");
  xml << "<?xml version=\"1.0\"?>"
      << "<!DOCTYPE predictor-info SYSTEM \"predictor-info.dtd\">"
      << "<predictor-info>";

  FILE * const raw = fopenRead(PredStats::filename);
  pred_info stats;
  while (read_pred_full(raw, stats))
    {
      const unit_t &unit = units[stats.unitIndex];
      const site_t &site = sites[stats.siteIndex];
      const string &scheme = scheme_name(unit.scheme_code);

      string filename(site.file);
      for (list<string>::const_iterator prefix = prefixes.begin(); prefix != prefixes.end(); ++prefix)
	if (filename.compare(0, prefix->size(), *prefix) == 0)
	  {
	    filename.erase(0, prefix->size());
	    break;
	  }

      xml << "<info unit=\"" << unit.signature
	  << "\" scheme=\"" << scheme
	  << "\" site=\"" << stats.siteIndex + 1
	  << "\" predicate=\"" << stats.predicate
	  << "\" file=\"" << filename
	  << "\" line=\"" << site.line
	  << "\" function=\"" << site.fun
	  << "\" cfg-node=\"" << site.cfg_node;

      if (rareSites.find(stats) != rareSites.end())
	xml << "\" class=\"rare";

      xml << "\">"

	  << "<bug-o-meter true-success=\"" << stats.s
	  << "\" true-failure=\"" << stats.f
	  << "\" seen-success=\"" << stats.os
	  << "\" seen-failure=\"" << stats.of
	  << "\" fail=\"" << stats.ps.fs
	  << "\" context=\"" << stats.ps.co
	  << "\" increase=\"" << stats.ps.in
	  << "\" lower-bound=\"" << stats.ps.lb
	  << "\" log10-true=\"" << log10(double(stats.s + stats.f))
	  << "\"/>";

      switch (units[stats.unitIndex].scheme_code)
	{
	case 'B':
	case 'G':
	case 'R':
	  xml << "<operand source=\"" << site.args[0] << "\"/>";
	  break;
	case 'S':
	  xml << "<operand source=\"" << site.args[0] << "\"/>"
	      << "<operand source=\"" << site.args[1] << "\"/>";
	  break;
	}

      xml << "</info>";
    }

  xml << "</predictor-info>\n";
}
