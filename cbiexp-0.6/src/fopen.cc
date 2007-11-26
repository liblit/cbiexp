#include <cerrno>
#include "fopen.h"

using namespace std;


static FILE *fopenCheck(const char *filename, const char mode[], const char verb[])
{
  FILE * const fp = fopen(filename, mode);
  if (!fp) {
    const int code = errno;
    fprintf(stderr, "cannot %s %s: %s\n", verb, filename, strerror(code));
    exit(code || 1);
  }

  return fp;
}


////////////////////////////////////////////////////////////////////////


FILE *fopenRead(const char *filename)
{
  return fopenCheck(filename, "r", "read");
}


FILE *fopenRead(const string &filename)
{
  return fopenRead(filename.c_str());
}


////////////////////////////////////////////////////////////////////////


FILE *fopenWrite(const char *filename)
{
  return fopenCheck(filename, "w", "write");
}


FILE *fopenWrite(const string &filename)
{
  return fopenWrite(filename.c_str());
}
