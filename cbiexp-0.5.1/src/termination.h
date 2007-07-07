#ifndef INCLUDE_cbiexp_src_termination_h
#define INCLUDE_cbiexp_src_termination_h


#include "config.h"


#ifdef HAVE_GNU_CXX_VERBOSE_TERMINATE_HANDLER
#  define set_terminate_verbose()  set_terminate(__gnu_cxx::__verbose_terminate_handler)
#else // no GNU C++ verbose terminate handler
#  define set_terminate_verbose()
#endif


#endif // !INCLUDE_cbiexp_src_termination_h
