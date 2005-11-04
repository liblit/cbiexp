#include <fstream>
#include <iterator>
#include "../StaticSiteInfo.h"
#include "Locations.h"

using namespace std;


void
Locations::dump(const char kind[]) const
{
  string sinkName("never-");
  sinkName += kind;
  sinkName += ".txt";
  ofstream sink(sinkName.c_str());

  copy(begin(), end(), ostream_iterator<Location>(sink, ""));
}
