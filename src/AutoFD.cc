#include <cerrno>
#include <stdexcept>
#include <unistd.h>
#include "AutoFD.h"

using namespace std;


AutoFD::AutoFD(int fd)
  : fd(fd)
{
  if (fd < 0)
    throw runtime_error(strerror(errno));
}


AutoFD::~AutoFD()
{
  if (close(fd) != 0)
    throw runtime_error(strerror(errno));
}
