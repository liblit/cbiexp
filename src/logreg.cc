#include <argp.h>
#include <iostream>
#include <iterator>
#include <set>
#include "PredCoords.h"
#include "StaticSiteInfo.h"
#include "utils.h"

using namespace std;


static StaticSiteInfo staticSiteInfo;


////////////////////////////////////////////////////////////////////////
//
//  a single predictor with an importance score
//


struct Predictor : public PredCoords
{
  double importance;
};


static istream &
operator>>(istream &in, Predictor &predictor)
{
  return in >> reinterpret_cast<PredCoords &>(predictor)
	    >> predictor.importance;
}


static ostream &
operator<<(ostream &out, const Predictor &predictor)
{
  const site_t &site = staticSiteInfo.site(predictor);

  out << "<predictor "
      << "scheme=\"" << scheme_name(site.scheme_code)
      << "\" file=\"" << site.file
      << "\" line=\"" << site.line
      << "\" function=\"" << site.fun;

  // hack to get the right predicate offset
  switch (site.scheme_code)
  {
    case 'B':
    case 'F':
    case 'G':
      out << "\" predicate=\"" << predictor.predicate;
      break;
    case 'S':
    case 'R':
      out << "\" predicate=\"" << predictor.predicate*2;
      break;
  }
      
  out << "\" importance=\"" << predictor.importance
      << "\">";

  switch (site.scheme_code)
    {
    case 'B':
    case 'F':
    case 'G':
    case 'R':
      out << "<operand source=\"" << site.args[0] << "\"/>";
      break;
    case 'S':
      out << "<operand source=\"" << site.args[0] << "\"/>"
	  << "<operand source=\"" << site.args[1] << "\"/>";
      break;
    }

  out << "</predictor>";
  return out;
}


bool
operator<(const Predictor &a, const Predictor &b)
{
  return a.importance > b.importance
    || (a.importance == b.importance
	&& reinterpret_cast<const PredCoords &>(a) < reinterpret_cast<const PredCoords &>(b));
}


////////////////////////////////////////////////////////////////////////
//
//  The main event
//


int
main(int argc, char** argv)
{
  // command line processing and other initialization
  set_terminate(__gnu_cxx::__verbose_terminate_handler);
  argp_parse(0, argc, argv, 0, 0, 0);
  ios::sync_with_stdio(false);

  cout << fixed
       << "<?xml version=\"1.0\"?>"
       << "<?xml-stylesheet type=\"text/xsl\" href=\"logreg.xsl\"?>"
       << "<!DOCTYPE logreg SYSTEM \"logreg.dtd\">"
       << "<logreg>";

  set<Predictor> predictors;
  copy(istream_iterator<Predictor>(cin), istream_iterator<Predictor>(), inserter(predictors, predictors.begin()));
  copy(predictors.begin(), predictors.end(), ostream_iterator<Predictor>(cout));

  cout << "</logreg>\n";

  return 0;
}
