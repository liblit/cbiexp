#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "reporting.h"


static void reportOnce(int *, unsigned, const char[], ...)
     __attribute__((format(printf, 3, 4)));

static void reportOnce(int *seen, unsigned code, const char format[], ...)
{
  if (!*seen)
    {
      va_list args;
      *seen = 1;
      va_start(args, format);
      fprintf(stderr, "Error %u: ", code);
      vfprintf(stderr, format, args);
      va_end(args);
    }
}


/**********************************************************************/


void check_bug_1(const char *yytext, int yyleng)
{
  static int seen;

  if (memchr(yytext, '\n', yyleng))
    reportOnce(&seen, 1, "newline in C comment and matching on.\n Output may be erroneous\n");
}


void check_bug_2(void *f)
{
  static int seen;

  if (!f)
    reportOnce(&seen, 2, "Failed to check return value of fopen in write_database and fopen failed.\n");
}


void check_bug_3()
{
  static unsigned databaseCount;
  static int seen;

  if (++databaseCount > 1)
    reportOnce(&seen, 3, "may fail because more than one database is loaded.\n");
}


void check_bug_4(int token_window_size, int token_window_size_max)
{
  static int seen;

  if (token_window_size >= token_window_size_max)
    reportOnce(&seen, 4, "could fail with buffer overrun.\n");
}


void check_bug_5(void *p)
{
  static int seen;

  if (!p)
    reportOnce(&seen, 5, "missing null pointer check.\n");
}


void check_bug_6a(int pindex, int passage_array_size)
{
  static int seen;

  if (pindex >= passage_array_size)
    reportOnce(&seen, 6, "Program may die or have incorrect results.\n");
}


void check_bug_6b(int i)
{
  static int seen;

  if (i != 0)
    reportOnce(&seen, 6, "-p with small memory.\n");
}


void check_bug_7(int i, int n, int pindex)
{
  static int seen;

  if (i + n >= pindex)
    reportOnce(&seen, 7, "Missing array bounds check in nth_duplicate_in_file happened.\n");
}


void check_bug_8(int j, int pindex)
{
  static int seen;

  if (j >= pindex)
    reportOnce(&seen, 8, "Missing array bounds check in which_duplicate_in_file happened.\n");
}


void check_bug_9(int winnowing_window_size, int winnowing_window_size_max)
{
  static int seen;

  if (winnowing_window_size > winnowing_window_size_max)
    reportOnce(&seen, 9, "Winnowing window size is too large, could cause buffer overrun.\n");
}
