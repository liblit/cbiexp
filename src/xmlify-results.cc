#include <argp.h>
#include <cmath>
#include <ext/hash_set>
#include <fstream>
#include "PredStats.h"
#include "fopen.h"
#include "sites.h"
#include "units.h"
#include "utils.h"

using namespace std;
using __gnu_cxx::hash_set;


////////////////////////////////////////////////////////////////////////
//
//  The main event
//


int main(int argc, char *argv[])
{
  argp_parse(0, argc, argv, 0, 0, 0);
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

      xml << "<predictor scheme=\"" << scheme
	  << "\" file=\"" << site.file
	  << "\" line=\"" << site.line
	  << "\" function=\"" << site.fun
	  << "\" predicate=\"" << stats.predicate
	  << "\" lower-bound=\"" << stats.ps.lb
	  << "\" increase=\"" << stats.ps.in
	  << "\" fail=\"" << stats.ps.fs
	  << "\" context=\"" << stats.ps.co
	  << "\" true-success=\"" << stats.s
	  << "\" true-failure=\"" << stats.f
	  << "\" seen-success=\"" << stats.os
	  << "\" seen-failure=\"" << stats.of
	  << "\" log10-true=\"" << log10(double(stats.s + stats.f))
	  << "\">";

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

      xml << "</predictor>";
    }

  xml << "</predictor-info>\n";
}
