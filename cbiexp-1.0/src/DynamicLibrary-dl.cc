#include <stdexcept>
#include "DynamicLibrary-dl.h"

using namespace std;


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
