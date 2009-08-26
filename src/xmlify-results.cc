#include <cmath>
#include <fstream>
#include <list>
#include "PredStats.h"
#include "SiteCoords.h"
#include "StaticSiteInfo.h"
#include "arguments.h"
#include "fopen.h"
#include "utils.h"
#include "xml.h"

using namespace std;

using xml::escape;

////////////////////////////////////////////////////////////////////////
//
//  The main event
//


int main()
{
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

      xml << "<info scheme=\"" << scheme
	  << "\" site=\"" << stats.siteIndex + 1
	  << "\" predicate=\"" << stats.predicate
	  << "\" file=\"" << filename
	  << "\" line=\"" << site.line
	  << "\" function=\"" << escape(site.fun)
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
	case 'A':
	case 'B':
	case 'F':
	case 'G':
	case 'R':
	case 'Y':
	case 'Z':
	case 'C':
	case 'W':
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
