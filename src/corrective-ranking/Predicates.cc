#include <cassert>
#include <cerrno>
#include <fstream>
#include <iostream>

#include "../Progress/Bounded.h"
#include "../Progress/Unbounded.h"

#include "Predicates.h"

using namespace std;


Predicates::Predicates()
  : count(0)
{
}


static void
open(ifstream &stream, const char directory[], const char filename[])
{
  string path(directory);
  path += '/';
  path += filename;

  stream.open(path.c_str());

  if (!stream)
    {
      const int error = errno;
      cerr << "cannot read " << path << ": " << strerror(error) << endl;
      exit(error || 1);
    }
}


void Predicates::load(const char directory[])
{
  ifstream tru, obs;
  open(tru, directory, "tru.txt");
  open(obs, directory, "obs.txt");

  Progress::Unbounded progress("reading predicates");
  while (tru && obs)
    {
      // speculate that there is at least one more predicate so we can
      // create space for it first and then build it in place
      push_back(count++);
      back().load(tru, obs);
      if (tru && obs) progress.step();
    }

  // last speculation was wrong
  pop_back();
  --count;

  assert(tru.eof());
  assert(obs.eof());
}


void
Predicates::rescore(Progress::Bounded &progress)
{
  iterator loser = begin();
  while (loser != end())
    if (loser->lowerBound() > 0)
      {
	loser->effective = loser->score();
	++loser;
      }
    else
      {
	progress.step();
	loser = erase(loser);
      }
}
