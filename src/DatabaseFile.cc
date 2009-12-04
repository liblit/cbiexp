#include "DatabaseFile.h"


const char *DatabaseFile::DatabaseName = "reports.sqlite3";


#ifdef HAVE_ARGP_H

static const argp_option options[] = {
  {
    "database-filename",
    'd',
    "SQLITE-DATQABASE",
    0,
    "Name of input Sqlite3 database file.  (default: reports.sqlite3)",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};


static int
parseFlag(int key, char *arg, argp_state *)
{
  switch (key)
    {
    case 'd':
      DatabaseFile::DatabaseName = arg;
      return 0;
    default:
      return ARGP_ERR_UNKNOWN;
    }
}


const argp DatabaseFile::argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};

#endif // HAVE_ARGP_H
