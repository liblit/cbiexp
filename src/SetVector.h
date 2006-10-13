#ifndef INCLUDE_SetVector_h
#define INCLUDE_SetVector_h

#include <iostream>
#include <vector>
#include <set>
#include <boost/dynamic_bitset.hpp>

using namespace std;

typedef boost::dynamic_bitset<> imp;
typedef imp::size_type size_type;

class SetVector : private boost::dynamic_bitset<> 
{
public:
    void resize(size_type, bool val = false);
    void set(size_type, bool val = true);
    size_type size() const;
    bool test(size_type) const;
    void print(ostream &) const;
    void load(istream &);
    void load(vector <bool> &);
    friend void computeOR(const SetVector &, const SetVector &, SetVector &);
    friend void computeAnd(const SetVector &, const SetVector &, SetVector &);
    friend bool nonEmptyIntersection(const SetVector &, const SetVector &);
};


ostream & operator<<(ostream &, const SetVector &);
istream & operator>>(istream &, SetVector &); 

#endif // !INCLUDE_SetVector_h

