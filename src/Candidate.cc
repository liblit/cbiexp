#include <iostream>
#include "Candidate.h"
#include "PredicatePrinter.h"


std::ostream &
operator << (std::ostream &out, const Candidate &candidate)
{
  PredicatePrinter printer(out, candidate);
  return out << "<popularity initial=\"" << candidate.firstImpression
	     << "\" effective=\"" << candidate.popularity
	     << "\"/>";
}
