#ifndef INCLUDE_PredicatePrinter_h
#define INCLUDE_PredicatePrinter_h

#include <iosfwd>
#include <vector>
#include "utils.h"


class PredicatePrinter
{
public:
  PredicatePrinter(std::ostream &, const pred_info &);
  ~PredicatePrinter();

private:
  // unimplemented and forbidden
  PredicatePrinter(const PredicatePrinter &);

  std::ostream &out;
};


std::ostream & operator << (std::ostream &, const pred_info &);


#endif // !INCLUDE_PredicatePrinter_h
