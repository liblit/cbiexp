#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <sstream>
#include <sys/stat.h>
#include "../Progress/Bounded.h"
#include "../Stylesheet.h"
#include "Predicates.h"
#include "allFailures.h"
#include "zoom.h"

using namespace std;


struct MoreDilutedThan : public binary_function<const Predicate &, const Predicate &, bool>
{
  bool operator()(const Predicate &, const Predicate &);
};


bool
MoreDilutedThan::operator()(const Predicate &a, const Predicate &b)
{
  const double diluteA = a.initial - a.effective;
  const double diluteB = b.initial - b.effective;
  return diluteA > diluteB
    || (diluteA == diluteB && a.index < b.index);
}


typedef set<Predicate, MoreDilutedThan> Neighbors;


void
buildZooms(const Predicates &candidates, const char projection[])
{
  Progress::Bounded progress("building zoom views", candidates.count);

  const RunSet originalAllFailures(allFailures);

  for (Predicates::const_iterator winner = candidates.begin(); winner != candidates.end(); ++winner)
    {
      progress.step();

      allFailures.dilute(*winner, winner->tru.failures);

      Neighbors losers;
      for (Predicates::const_iterator loser = candidates.begin(); loser != candidates.end(); ++loser)
	{
	  Predicate copy(*loser);
	  copy.dilute(*winner);
	  copy.effective = copy.lowerBound() > 0 ? copy.score() : 0;
	  losers.insert(copy);
	}

      ostringstream filename;
      filename << "zoom-" << projection << '-' << winner->index + 1 << ".xml";
      ofstream view(filename.str().c_str());
      view.exceptions(ios::badbit | ios::failbit);

      view << "<?xml version=\"1.0\"?>"
	   << "<?xml-stylesheet type=\"text/xsl\" href=\"" << Stylesheet::filename << "\"?>"
	   << "<!DOCTYPE scores SYSTEM \"corrected-view.dtd\">"
	   << "<scores scheme=\"all\" sort=\"zoom\" projection=\""
	   << projection
	   << "\">";
      copy(losers.begin(), losers.end(), ostream_iterator<Predicate>(view));
      view << "</scores>\n";

      allFailures = originalAllFailures;
    }
}
