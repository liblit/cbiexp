#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

#include "CountingIterator.h"
#include "Predicate.h"
#include "RunSet.h"

using namespace std;


istream &
operator>>(istream &in, RunSet &runs)
{
  assert(runs.empty());
  runs.count = 0;

  unsigned runId;
  in.exceptions(ios::badbit);
  while (in >> runId)
    {
      runs.push_back(runId);
      ++runs.count;
    }

  return in;
}


RunSet::RunSet(Outcome outcome)
  : outcome(outcome)
{
}


void
RunSet::load(istream &in)
{
  string actual;
  in >> actual;
  if (in)
    {
      assert(actual.size() == 2);
      assert(actual[0] == (outcome == Failure ? 'F' : 'S'));
      assert(actual[1] == ':');

      string line;
      getline(in, line);

      istringstream parse(line);
      parse >> *this;
    }
}


size_t
RunSet::intersectionSize(const RunSet &other)
{
  size_t result = 0;

  CountingIterator<unsigned> sink(result);
  set_intersection(begin(), end(), other.begin(), other.end(), sink);

  return result;
}
