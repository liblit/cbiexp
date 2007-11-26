#include <stdexcept>
#include "DynamicLibrary-win.h"

using std::runtime_error;


DynamicLibrary::DynamicLibrary(const char filename[])
  : handle(LoadLibrary(filename))
{
  if (!handle)
    throwError();
}


DynamicLibrary::~DynamicLibrary()
{
  if (handle)
    if (!FreeLibrary(handle))
      throwError();
}


void
DynamicLibrary::throwError() const
{
  const DWORD errorCode = GetLastError();
  LPSTR buffer;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
	0, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	reinterpret_cast<LPSTR>(&buffer), 0, 0);

  const std::string message(buffer);
  LocalFree(buffer);

  throw std::runtime_error(message);
}
