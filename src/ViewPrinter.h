#ifndef INCLUDE_ViewPrinter_h
#define INCLUDE_ViewPrinter_h

#include <fstream>
#include <string>


class ViewPrinter : public std::ofstream
{
public:
  ViewPrinter(const char stylesheet[], const char dtd[], const char filename[]);
  ViewPrinter(const char stylesheet[], const char dtd[], const std::string &scheme, const char sort[], const char projection[]);
  ~ViewPrinter();

private:
  void init(const char filename[], const char stylesheet[], const char dtd[]);
};


#endif // !INCLUDE_ViewPrinter_h
