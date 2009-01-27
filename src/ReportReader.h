#ifndef INCLUDE_ReportReader_h
#define INCLUDE_ReportReader_h

#include <vector>
#include "Counts.h"
#include <iostream>
#include <fstream>

using namespace std;

class SiteCoords;

class ReportReader
{
public:
  ReportReader(const char* filename);
  ifstream summary; //the summary file from which reports are read
  unsigned linepos; //the line position within the file
  virtual ~ReportReader() { }
  void read(unsigned runId);


protected:
  virtual void handleSite(const SiteCoords &, std::vector<count_tp> &) = 0;

};


#endif // !INCLUDE_ReportReader_h
