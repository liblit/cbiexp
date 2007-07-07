#ifndef INCLUDE_Rho_h
#define INCLUDE_Rho_h

#include "MappedArray.h"
#include "Matrix.h"


////////////////////////////////////////////////////////////////////////
//
//  Pairwise predicate correlation matrix
//


class Rho
{
public:
  Rho(size_t size);

  const double * operator [] (size_t) const;

private:
  const MappedArray<double> mapping;
  const Matrix<const double> matrix;
};


////////////////////////////////////////////////////////////////////////


inline const double *
Rho::operator [] (size_t index) const
{
  return matrix[index];
}


#endif // !INCLUDE_Rho_h
