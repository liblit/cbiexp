#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "reporting.h"


enum { LastErrorCode = 9 };


static void va_reportError(unsigned code, const char format[], va_list args)
{
  static int alreadyReported[LastErrorCode + 1];

  assert(code <= LastErrorCode);

  if (!alreadyReported[code])
    {
      alreadyReported[code] = 1;
      fprintf(stderr, "Error %u: ", code);
      vfprintf(stderr, format, args);
    }
}


void reportError(unsigned code, const char format[], ...)
{
  va_list args;
  va_start(args, format);
  va_reportError(code, format, args);
  va_end(args);
}


void reportErrorIf(int guard, unsigned code, const char format[], ...)
{
  if (guard)
    {
      va_list args;
      va_start(args, format);
      va_reportError(code, format, args);
      va_end(args);
    }
}


/**********************************************************************/


void checkNewlineInCommentError()
{
  extern int yyleng;
  extern char *yytext;

  reportErrorIf(!!memchr(yytext, '\n', yyleng),
		1, "newline in C comment and matching on.\n Output may be erroneous\n");
}


void checkDatabaseCountError()
{
  static unsigned databaseCount;

  ++databaseCount;
  reportErrorIf(databaseCount > 1,
		3, "may fail because more than one database is loaded.\n");
}
