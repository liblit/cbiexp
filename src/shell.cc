// -*- c-file-style: "gnu" -*-

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include "shell.h"


bool verbose = false;


void shell(const char format[], ...)
{
  va_list args;
  va_start(args, format);

  char *command;
  vasprintf(&command, format, args);

  if (verbose)
    fprintf(stderr, "+ %s\n", command);

  const int result = system(command);
  free(command);

  if (result)
    exit(WTERMSIG(result) || WEXITSTATUS(result) || 1);

  va_end(args);
}
