#ifndef INCLUDE_Matrix_h
#define INCLUDE_Matrix_h

#include <cassert>


// access a single block of memory as though it were a 2D matrix


template <typename Element>
class Matrix
{
public:
  Matrix(size_t, Element *);

  const size_t size;

  Element * operator [] (size_t);
  const Element * operator [] (size_t) const;

 private:
  Element * const storage;
};


////////////////////////////////////////////////////////////////////////


template <typename Element>
inline
Matrix<Element>::Matrix(size_t size, Element *storage)
  : size(size),
    storage(storage)
{
}


template <typename Element>
inline Element *
Matrix<Element>::operator [] (size_t index)
{
  assert(index < size);
  return storage + size * index;
}


template <typename Element>
inline const Element *
Matrix<Element>::operator [] (size_t index) const
{
  assert(index < size);
  return storage + size * index;
}


#endif // !INCLUDE_Matrix_h
