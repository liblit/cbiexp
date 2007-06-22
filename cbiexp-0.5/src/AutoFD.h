#ifndef INCLUDE_AutoFD_h
#define INCLUDE_AutoFD_h

#include <cassert>


////////////////////////////////////////////////////////////////////////
//
//  A file descriptor with automatic cleanup
//


class AutoFD
{
public:
  AutoFD(int);
  ~AutoFD();

  operator int () const;

private:
  const int fd;

  // unimplemented and forbidden
  AutoFD(const AutoFD &);
};


////////////////////////////////////////////////////////////////////////


inline
AutoFD::operator int () const
{
  return fd;
}


#endif // !INCLUDE_AutoFD_h
