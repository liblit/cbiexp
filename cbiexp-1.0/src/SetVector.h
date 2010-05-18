#ifndef INCLUDE_SetVector_h
#define INCLUDE_SetVector_h

#include <iostream>
#include <vector>
#include <set>
#include <boost/dynamic_bitset.hpp>

using namespace std;

typedef boost::dynamic_bitset<> imp;

class SetVector
{
public:
    virtual ~SetVector();
    void resize(size_t, bool val = false);
    virtual void set(size_t, bool val = true);
    size_t count() const;
    const imp & value() const; 
    imp copy() const;
    bool test(size_t) const;
    void print(ostream &) const;
    virtual void load(istream &);
    virtual void load(const imp &);
    virtual void load(const vector <bool> &);
    vector <unsigned int> toCounts() const;
protected:
    imp theVector; 
};

ostream & operator<<(ostream &, const SetVector &);
istream & operator>>(istream &, SetVector &); 

#endif // !INCLUDE_SetVector_h

