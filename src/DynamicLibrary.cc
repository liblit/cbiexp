#include <stdexcept>
#include "DynamicLibrary.h"

using std::runtime_error;


DynamicLibrary::DynamicLibrary(const char filename[])
  : handle(dlopen(filename, RTLD_NOW))
{
  if (!handle)
    throw runtime_error(dlerror());
}


DynamicLibrary::~DynamicLibrary()
{
  if (handle)
    if (dlclose(handle))
      throw runtime_error(dlerror());
}
