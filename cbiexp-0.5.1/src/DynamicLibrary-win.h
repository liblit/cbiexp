#ifndef INCLUDE_DynamicLibrary_win_h
#define INCLUDE_DynamicLibrary_win_h

#include <string>
#include <windows.h>


#define DYNAMIC_LIBRARY_NAME(base) (".\\" base ".dll")


class DynamicLibrary
{
public:
  DynamicLibrary(const char []);
  ~DynamicLibrary();

  template<class T> T lookup(const char []) const;

private:
  HINSTANCE handle;

  void throwError() const;

  // forbidden
  DynamicLibrary(const DynamicLibrary &);
  DynamicLibrary &operator=(const DynamicLibrary &);
};


////////////////////////////////////////////////////////////////////////


template<class T>
T
DynamicLibrary::lookup(const char symbol[]) const
{
  typedef T (*Provider)();
  const Provider provider = reinterpret_cast<Provider>(GetProcAddress(handle, symbol));

  if (!provider)
	  throwError();

  return provider();
}


#endif // !INCLUDE_DynamicLibrary_win_h
