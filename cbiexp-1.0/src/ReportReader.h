#ifndef INCLUDE_ReportReader_h
#define INCLUDE_ReportReader_h

#include "Counts.h"
#include <sqlite3.h>
#include <vector>

using namespace std;

class SiteCoords;

class ReportReader
{
public:
  ReportReader(const char* filename);
  string dbfile; //the sqlite database
  sqlite3 *db;

  virtual ~ReportReader() { }
  void read(unsigned runId);

protected:
  virtual void handleSite(const SiteCoords &, std::vector<count_tp> &) = 0;

private:
  void throwError(const char *msg, const char *file, int line);
};


#endif // !INCLUDE_ReportReader_h
