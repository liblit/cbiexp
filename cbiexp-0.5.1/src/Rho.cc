#include <cassert>
#include "Rho.h"

using namespace std;


Rho::Rho(const size_t size)
  : mapping("rho.bin"),
    matrix(size, mapping.begin())
{
  assert(size * size == mapping.size());
}
