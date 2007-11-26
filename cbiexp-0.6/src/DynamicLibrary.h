#ifndef INCLUDE_DynamicLibrary_h
#define INCLUDE_DynamicLibrary_h


#include "config.h"


#ifdef HAVE_DLFCN_H
#  include "DynamicLibrary-dl.h"
#else
#  include "DynamicLibrary-win.h"
#endif


#endif // !INCLUDE_DynamicLibrary_h
