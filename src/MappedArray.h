#ifndef INCLUDE_MappedArray_h
#define INCLUDE_MappedArray_h

#include <cstddef>


// map a file into memory as though it were an array


template <typename Element>
class MappedArray
{
public:
  MappedArray(const char []);
  ~MappedArray();

  size_t size() const;
  const Element *begin() const;
  const Element *end() const;
  const Element * operator [] (size_t) const;

 private:
  size_t length() const;

  size_t count;
  Element *storage;

  // unimplemented and forbidden
  MappedArray(const MappedArray<Element> &);
};


////////////////////////////////////////////////////////////////////////


#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "AutoFD.h"


template <typename Element>
MappedArray<Element>::MappedArray(const char filename[])
{
  const AutoFD fd(open(filename, O_RDONLY));

  struct stat stat;
  const int error = fstat(fd, &stat);
  assert(!error);

  count = stat.st_size / sizeof(Element);
  storage = reinterpret_cast<Element *>(mmap(0, length(), PROT_READ, MAP_SHARED, fd, 0));
}


template <typename Element>
MappedArray<Element>::~MappedArray()
{
  const int error = munmap(storage, length());
  assert(!error);
}


template <typename Element>
size_t
MappedArray<Element>::size() const
{
  return count;
}


template <typename Element>
size_t
MappedArray<Element>::length() const
{
  return count * sizeof(Element);
}


template <typename Element>
const Element *
MappedArray<Element>::begin() const
{
  return storage;
}


template <typename Element>
const Element *
MappedArray<Element>::end() const
{
  return storage + length();
}


template <typename Element>
const Element *
MappedArray<Element>::operator [] (size_t index) const
{
  assert(index < count);
  return storage + index;
}


#endif // !INCLUDE_MappedArray_h
