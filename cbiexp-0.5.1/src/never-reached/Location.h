#ifndef INCLUDE_Location_h
#define INCLUDE_Location_h

#include <iosfwd>
#include <string>

class site_t;


class Location
{
public:
  Location(const site_t &);

  std::string file;
  unsigned line;

  // presented for reference, but not part of key
  std::string func;
};




namespace __gnu_cxx
{
  template <> struct hash<Location>
  {
    size_t operator()(const Location &) const;
  };
}


bool operator==(const Location &, const Location &);

std::ostream &operator<<(std::ostream &, const Location &);


#endif // !INCLUDE_Location_h
