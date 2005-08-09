#include <cassert>
#include <iostream>
#include <iterator>
#include "../Units.h"
#include "../utils.h"
#include "Both.h"
#include "Candidates.h"
#include "Predicate.h"

using namespace std;


static Units units;


////////////////////////////////////////////////////////////////////////


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


static ostream &
operator<<(ostream &out, Reason reason)
{
  switch (reason)
    {
    case NotBetter:
      return out << "not better";
    case FirstInList:
      return out << "first in list";
    case HighestScore:
      return out << "highest score";
    case NonScalarPairs:
      return out << "not scalar-pairs";
    case NonDerived:
      return out << "not derived (logically stronger)";
    default:
      return out << "unknown reason";
    }
}


static Reason
better(const Candidates::value_type &challenger, double challengerScore,
       const Candidates::value_type &leader, double leaderScore)
{
  if (challengerScore < leaderScore)
    return NotBetter;

  if (challengerScore > leaderScore)
    return HighestScore;

  // !!!: deviation from formal definitions:
  //   prefer non-scalar-pairs predictors
  if (units[challenger.first.unitIndex].scheme_code != 'S' &&
      units[leader    .first.unitIndex].scheme_code == 'S')
    return NonScalarPairs;

  // !!!: deviation from formal definitions:
  //   prefer non-derived predictors, since they are logically
  //   stronger than derived predictors
  if (challenger.first.predicate % 2 == 0 &&
      leader    .first.predicate % 2 != 0)
    return NonDerived;

  return NotBetter;
}


Candidates::const_iterator
Candidates::best() const
{
  // It would be nice to use std::max_element here instead of looping
  // ourselves, but then we would no longer be able to cache the score
  // of the current leader.  Scores are costly enough to recompute that
  // I don't want to give up that performance boost.

  assert(begin() != end());

  const_iterator leader = begin();
  double leaderScore = leader->second->score();
  Reason leaderReason = FirstInList;

  const_iterator challenger = leader;
  while (++challenger != end())
    {
      const double challengerScore = challenger->second->score();
      const Reason challengerReason = better(*challenger, challengerScore,
					     *leader, leaderScore);

      if (challengerReason != NotBetter)
	{
	  leader = challenger;
	  leaderScore = challengerScore;
	  leaderReason = challengerReason;
	}
    }

  cerr << "next best candidate selected due to: " << leaderReason << '\n';
  return leader;
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
