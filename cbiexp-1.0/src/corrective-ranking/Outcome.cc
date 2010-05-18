#include <cstdlib>
#include <iostream>
#include "Outcome.h"


std::ostream &
operator<<(std::ostream &sink, Outcome outcome)
{
  switch (outcome)
    {
    case Failure:
      return sink << "Failure";
    case Success:
      return sink << "Success";
    default:
      abort();
    }
}
