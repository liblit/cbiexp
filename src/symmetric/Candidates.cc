#include <cassert>
#include "Candidates.h"
#include "Predicate.h"


static bool
trueInNoFailures(const Candidates::value_type &chaff)
{
  return chaff.second->trueInFailures.empty();
}


void
Candidates::filter()
{
  __gnu_cxx::hash_map<Coords, Predicate *> replacement;
  remove_copy_if(begin(), end(), inserter(replacement, replacement.begin()), trueInNoFailures);
  swap(replacement);
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
      if (challengerScore > winnerScore)
	{
	  winner = challenger;
	  winnerScore = challengerScore;
	}
    }

  return winner;
}
