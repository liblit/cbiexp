#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

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


void
RunSet::load(istream &in, char expected)
{
  string actual;
  in >> actual;
  if (in)
    {
      assert(actual.size() == 2);
      assert(actual[0] == expected);
      assert(actual[1] == ':');

      string line;
      getline(in, line);

      istringstream parse(line);
      parse >> *this;
    }
}
