#ifndef INCLUDE_ViewPrinter_h
#define INCLUDE_ViewPrinter_h

#include <fstream>
#include <string>


class ViewPrinter : public std::ofstream
{
public:
  ViewPrinter(const char *stylesheet, const char dtd[], const std::string &scheme, const char sort[], const char projection[]);
  ~ViewPrinter();
};


#endif // !INCLUDE_ViewPrinter_h
