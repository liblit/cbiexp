#ifndef INCLUDE_DynamicLibrary_h
#define INCLUDE_DynamicLibrary_h

#include <dlfcn.h>
#include <stdexcept>


class DynamicLibrary
{
public:
  DynamicLibrary(const char []);
  ~DynamicLibrary();

  template<class T> T &lookup(const char []) const;

private:
  void * const handle;

  // forbidden
  DynamicLibrary(const DynamicLibrary &);
  DynamicLibrary &operator=(const DynamicLibrary &);
};


////////////////////////////////////////////////////////////////////////


template<class T>
T &
DynamicLibrary::lookup(const char symbol[]) const
{
  T * const address = reinterpret_cast<T *>(dlsym(handle, symbol));

  const char * const error = dlerror();
  if (error)
    throw std::runtime_error(error);

  return *address;
}


#endif // !INCLUDE_DynamicLibrary_h
