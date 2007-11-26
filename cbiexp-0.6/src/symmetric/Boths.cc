#include "Boths.h"


Boths::~Boths()
{
  for (iterator chaff = begin(); chaff != end(); ++chaff)
    delete *chaff;
}
