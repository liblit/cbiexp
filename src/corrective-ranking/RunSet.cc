#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

#include "../NumRuns.h"

#include "Predicate.h"
#include "RunSet.h"

using namespace std;


istream &
operator>>(istream &in, RunSet &runs)
{
  runs.assign(NumRuns::end, false);
  runs.count = 0;

  unsigned runId;
  in.exceptions(ios::badbit);
  while (in >> runId)
    {
      runs[runId] = 1;
      ++runs.count;
    }

  return in;
}


RunSet::RunSet(Outcome outcome)
  : outcome(outcome),
    count(0)
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
  assert(size() == other.size());

  size_t result = 0;
  for (size_t runId = 0; runId < size(); ++runId)
    if ((*this)[runId] && other[runId])
      ++result;

  return result;
}
