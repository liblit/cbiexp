#include <iostream>
#include "Candidate.h"


std::ostream &
operator << (std::ostream &out, const Candidate &candidate)
{
  return out << "<predictor index=\"" << candidate.index + 1
	     << "\" initial=\"" << candidate.firstImpression
	     << "\" effective=\"" << candidate.popularity
	     << "\"/>";
}
