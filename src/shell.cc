// -*- c-file-style: "gnu" -*-

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include "shell.h"


void shell(const char format[], ...)
{
  va_list args;
  va_start(args, format);

  char *command;
  vasprintf(&command, format, args);
  const int result = system(command);
  free(command);

  if (result)
    exit(result);

  va_end(args);
}
