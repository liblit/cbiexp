#include <iostream>
#include "generate-view.h"
#include "sites.h"

using namespace std;


PredicatePrinter::PredicatePrinter(ostream &out, const pred_info &stats)
  : out(out)
{
  out << "<predictor>"

      << "<true success=\"" << stats.s
      << "\" failure=\"" << stats.f << "\"/>"

      << "<seen success=\"" << stats.os
      << "\" failure=\"" << stats.of << "\"/>"

      << "<scores log10-seen=\"" << log10(double(stats.s + stats.f))
      << "\" context=\"" << stats.ps.co
      << "\" lower-bound=\"" << stats.ps.lb
      << "\" increase=\"" << stats.ps.in
      << "\" badness=\"" << stats.ps.fs
      << "\"/>"

      << "<source predicate=\"";

  const site_t &info = sites[stats.site];
  switch(info.scheme_code)
    {
    case 'S':
      {
	static const char* op[] = { "&lt;", "&gt;=", "==", "!=", "&gt;", "&lt;=" };
	out << info.args[0] << ' ' << op[stats.p] << ' ' << info.args[1];
	break;
      }
    case 'R':
      {
	static const char* op[] = { "&lt;", "&gt;=", "==", "!=", "&gt;", "&lt;=" };
	out << info.args[0] << ' ' << op[stats.p] << " 0";
	break;
      }
    case 'B':
      {
	const char* op[] = { "is FALSE", "is TRUE" };
	out << info.args[0] << ' ' << op[stats.p];
	break;
      }
    case 'G':
      {
	const char* op[] = { "= 0", "= 1", "&gt; 1", "invalid" };
	out << "old_refcount(" << info.args[0] << ") " << op[stats.p];
	break;
      }
    default:
      cerr << "unexpected scheme code: " << info.scheme_code << '\n';
      exit(1);
    }

  out << "\" function=\"" << info.fun
      << "\" file=\"" << info.file
      << "\" line=\"" << info.line
      << "\"/>";
}


PredicatePrinter::~PredicatePrinter()
{
  out << "</predictor>";
}


ostream &
operator << (ostream &out, const pred_info &stats)
{
  PredicatePrinter(out, stats);
  return out;
}
