#ifndef INCLUDE_generate_view_h
#define INCLUDE_generate_view_h

#include <iosfwd>
#include <vector>
#include "utils.h"


typedef size_t predIndex;
typedef std::vector<size_t> Permutation;

typedef std::vector<pred_info> PredStats;


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


#endif // !INCLUDE_generate_view_h
