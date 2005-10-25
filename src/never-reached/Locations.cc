#include <iterator>
#include "../StaticSiteInfo.h"
#include "Locations.h"

using namespace std;


void
Locations::dump(ostream &out) const
{
  copy(begin(), end(), ostream_iterator<Location>(out, ""));
}
