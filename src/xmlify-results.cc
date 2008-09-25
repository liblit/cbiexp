#include <cmath>
#include <fstream>
#include <list>
#include "PredStats.h"
#include "SiteCoords.h"
#include "StaticSiteInfo.h"
#include "arguments.h"
#include "fopen.h"
#include "termination.h"
#include "utils.h"
#include "xml.h"

using namespace std;

using xml::escape;


static list<string> prefixes;


////////////////////////////////////////////////////////////////////////
//
//  command line processing
//


#ifdef HAVE_ARGP_H
#include <sysexits.h>

static int
parseFlag(int key, char *arg, argp_state *)
{
  switch (key)
    {
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
    { "strip-prefix", 'p', "TEXT", 0, "remove TEXT from the start of file names; may be given multiple times", 0 },
    { 0, 0, 0, 0, 0, 0 }
  };

  static const argp argp = {
    options, parseFlag, 0, 0, 0, 0, 0
  };

  argp_parse(&argp, argc, argv, 0, 0, 0);
}

#else // !HAVE_ARGP_H

inline void
processCommandLine(int, char *[])
{
}

#endif // !HAVE_ARGP_H


////////////////////////////////////////////////////////////////////////
//
//  The main event
//


int main(int argc, char *argv[])
{
  set_terminate_verbose();
  processCommandLine(argc, argv);
  ios::sync_with_stdio(false);

  const StaticSiteInfo staticSiteInfo;

  ofstream xml("predictor-info.xml");
  xml << fixed
      << "<?xml version=\"1.0\"?>"
      << "<!DOCTYPE predictor-info SYSTEM \"predictor-info.dtd\">"
      << "<predictor-info>";

  FILE * const raw = fopenRead(PredStats::filename);
  pred_info stats;
  while (read_pred_full(raw, stats))
    {
      const site_t &site = staticSiteInfo.site(stats.siteIndex);
      const string &scheme = scheme_name(site.scheme_code);

      string filename(site.file);
      for (list<string>::const_iterator prefix = prefixes.begin(); prefix != prefixes.end(); ++prefix)
	if (filename.compare(0, prefix->size(), *prefix) == 0)
	  {
	    filename.erase(0, prefix->size());
	    break;
	  }

      xml << "<info scheme=\"" << scheme
	  << "\" site=\"" << stats.siteIndex + 1
	  << "\" predicate=\"" << stats.predicate
	  << "\" file=\"" << filename
	  << "\" line=\"" << site.line
	  << "\" function=\"" << site.fun
	  << "\" cfg-node=\"" << site.cfg_node
	  << "\">"

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

      switch (site.scheme_code)
	{
	case 'B':
	case 'F':
	case 'G':
	case 'R':
	  xml << "<operand source=\"" << escape(site.args[0]) << "\"/>";
	  break;
	case 'S':
	  xml << "<operand source=\"" << escape(site.args[0]) << "\"/>"
	      << "<operand source=\"" << escape(site.args[1]) << "\"/>";
	  break;
	}

      xml << "</info>";
    }

  xml << "</predictor-info>\n";
}
