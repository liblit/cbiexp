#ifndef INCLUDE_arguments_h
#define INCLUDE_arguments_h


#include "config.h"


#ifdef HAVE_ARGP_H
#  include <argp.h>
#else  // no <argp.h>
typedef void *argp;
#endif // no <argp.h>


#endif // !INCLUDE_arguments_h
