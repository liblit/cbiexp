#include <ext/hash_set>
#include "../site_t.h"
#include "Location.h"

using __gnu_cxx::hash;
using std::ostream;


Location::Location(const site_t &site)
  : file(site.file),
    line(site.line),
    func(site.fun)
{
}


size_t
hash<Location>::operator()(const Location &location) const
{
  // note that Location::func is not used for hashing or equality
  return location.line ^ hash<const char *>()(location.file.c_str());
}


bool
operator==(const Location &a, const Location &b)
{
  // note that Location::func is not used for hashing or equality
  return a.line == b.line
    && a.file == b.file;
}


ostream &
operator<<(ostream &out, const Location &location)
{
  return out << location.file << '\t'
	     << location.line << '\n';
}
