#ifndef INCLUDE_Matrix_h
#define INCLUDE_Matrix_h

#include <cassert>


// a single block of memory indexed as though it were a square matrix


template <typename Element>
class Matrix
{
public:
  Matrix(size_t);
  ~Matrix();

  Element *operator [] (size_t);
  const Element *operator [] (size_t) const;

  const size_t size;

 private:
  Element * const storage;
};


////////////////////////////////////////////////////////////////////////


template <typename Element>
inline
Matrix<Element>::Matrix(size_t size)
  : size(size),
    storage(new Element [size * size])
{
}


template <typename Element>
inline
Matrix<Element>::~Matrix()
{
  delete [] storage;
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
