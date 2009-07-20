#ifndef INCLUDE_ReportReader_h
#define INCLUDE_ReportReader_h

#include "Counts.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class SiteCoords;

class ReportReader
{
public:
  ReportReader(const char* filename);
  vector <streampos> positions; //vector of positions for each run
  string countfile; //the file from which reports are read
  virtual ~ReportReader() { }
  void read(unsigned runId);


protected:
  virtual void handleSite(const SiteCoords &, std::vector<count_tp> &) = 0;

};


#endif // !INCLUDE_ReportReader_h
