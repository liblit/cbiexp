#ifndef INCLUDE_generate_view_h
#define INCLUDE_generate_view_h

#include <iosfwd>
#include <vector>
#include "utils.h"


typedef size_t predIndex;
typedef std::vector<size_t> Permutation;

typedef std::vector<pred_info> PredStats;


std::ostream & operator << (std::ostream &, const pred_info &);

template <class Stats>
void generateView(std::ostream &,
		  const std::string &scheme, const char *sort,
		  const Stats &, const Permutation &);


////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <string>


template <class Iterator>
void generateView(std::ostream &out,
		  const std::string &scheme, const char *sort,
		  const Iterator &stats, const Permutation &order)
{
  out << "<?xml version=\"1.0\"?>"
      << "<?xml-stylesheet type=\"text/xsl\" href=\"view.xsl\"?>"
      << "<!DOCTYPE view SYSTEM \"view.dtd\">"
      << "<view scheme=\"" << scheme
      << "\" sort=\"" << sort << "\">";

  for (Permutation::const_iterator index = order.begin(); index != order.end(); ++index)
    out << *(stats + *index);

  out << "</view>\n";
}


#endif // !INCLUDE_generate_view_h
