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


enum Reason { NotBetter, FirstInList, HighestScore, NonScalarPairs, NonDerived };


static Reason
better(Candidates::value_type &challenger, double challengerScore, double winnerScore)
{
  if (challengerScore < winnerScore)
    return NotBetter;

  if (challengerScore > winnerScore)
    return HighestScore;

  // !!!: deviation from formal definitions:
  //   prefer non-scalar-pairs predictors
  if (units[challenger.first.unitIndex].scheme_code != 'S')
    return NonScalarPairs;

  // !!!: deviation from formal definitions:
  //   prefer non-derived predictors, which are logically stronger
  //   than derived predictors
  if (challenger.first.predicate & 1 == 0)
    return NonDerived;

  return NotBetter;
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
  Reason winnerReason = FirstInList;

  iterator challenger = winner;
  while (++challenger != end())
    {
      const double challengerScore = challenger->second->score();
      const Reason challengerReason = better(*challenger, challengerScore, winnerScore);
      if (challengerReason != NotBetter)
	{
	  winner = challenger;
	  winnerScore = challengerScore;
	  winnerReason = challengerReason;
	}
    }

  cerr << "next best candidate selected due to: ";
  switch (winnerReason)
    {
    case FirstInList:
      cerr << "first in list\n";
      break;
    case HighestScore:
      cerr << "highest score\n";
      break;
    case NonScalarPairs:
      cerr << "not scalar-pairs\n";
      break;
    case NonDerived:
      cerr << "not derived (logically stronger)\n";
      break;
    default:
      cerr << "unknown reason\n";
      exit(1);
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
