#include <cassert>
#include <iostream>
#include <iterator>
#include "../units.h"
#include "../utils.h"
#include "Both.h"
#include "Candidates.h"
#include "Predicate.h"

using namespace std;


static bool
trueInNoFailures(const Candidates::value_type &chaff)
{
  return chaff.second->trueInFailures.none();
}


void
Candidates::filter()
{
  __gnu_cxx::hash_map<PredCoords, Predicate *> replacement;
  remove_copy_if(begin(), end(), inserter(replacement, replacement.begin()), trueInNoFailures);
  swap(replacement);
}


////////////////////////////////////////////////////////////////////////


static bool
better(Candidates::value_type &challenger, double challengerScore, double winnerScore)
{
  if (challengerScore > winnerScore)
    return true;

  if (challengerScore < winnerScore)
    return false;

  // !!!: deviation from formal definitions:
  //   prefer non-scalar-pairs predictors
  if (units[challenger.first.unitIndex].scheme_code != 's')
    return true;

  return false;
}


Candidates::iterator
Candidates::best()
{
  // It would be nice to use std::max_element here instead of looping
  // ourselves, but then we would no longer be able to cache the score
  // of the current leader.  Scores are costly enough to recompute that
  // I don't want to give up that performance boost.

  assert(begin() != end());

  iterator winner = begin();
  double winnerScore = winner->second->score();

  iterator challenger = winner;
  while (++challenger != end())
    {
      const double challengerScore = challenger->second->score();
      if (better(*challenger, challengerScore, winnerScore))
	{
	  winner = challenger;
	  winnerScore = challengerScore;
	}
    }

  return winner;
}


////////////////////////////////////////////////////////////////////////


ostream &operator<<(ostream &out, const Candidates::value_type &winner)
{
  const PredCoords &coords = winner.first;
  const unit_t &unit = units[coords.unitIndex];
  const Predicate &pred = *winner.second;

  out << "<predictor unit=\"" << unit.signature
      << "\" scheme=\"" << scheme_name(unit.scheme_code)
      << "\" site=\"" << coords.siteOffset + 1
      << "\" predicate=\"" << coords.predicate + 1
      << "\" score=\"" << pred.score() << "\">";
  pred.print(out);
  out << "</predictor>";

  return out;
}


ostream &operator<<(ostream &out, const Candidates &candidates)
{
  copy(candidates.begin(), candidates.end(), ostream_iterator<Candidates::value_type>(out, ""));
  return out;
}
