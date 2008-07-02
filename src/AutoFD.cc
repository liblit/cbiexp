#include <cerrno>
#include <cstring>
#include <stdexcept>
#include "AutoFD.h"
#include "config.h"

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#else // !HAVE_UNISTD_H
#  include <io.h>
#endif // !HAVE_UNISTD_H

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
