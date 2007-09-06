#include <cassert>
#include <cerrno>
#include <fstream>
#include <iostream>

#include "../PredStats.h"
#include "../Progress/Bounded.h"

#include "Candidates.h"

using namespace std;


Candidates::Candidates()
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


void Candidates::load(const char directory[])
{
  ifstream tru, obs;
  open(tru, directory, "tru.txt");
  open(obs, directory, "obs.txt");

  const unsigned numPreds = PredStats::count();
  Progress::Bounded progress("reading predicates", numPreds);

  for (count = 0; count < numPreds; ++count)
    {
      progress.step();
      push_back(count);
      back().load(tru, obs);
    }

  assert(tru.peek() == EOF);
  assert(obs.peek() == EOF);
}
