#ifndef INCLUDE_SetVector_h
#define INCLUDE_SetVector_h

#include <iostream>
#include <vector>
#include <set>
#include <boost/dynamic_bitset.hpp>

using namespace std;

typedef boost::dynamic_bitset<> imp;
typedef imp::size_type size_type;

class SetVector
{
public:
    void resize(size_type, bool val = false);
    void set(size_type, bool val = true);
    size_type size() const;
    size_type count() const;
    imp value() const; 
    bool test(size_type) const;
    void print(ostream &) const;
    void load(istream &);
    void load(vector <bool> &);
    vector <unsigned int> toCounts() const;
protected:
    void computeOR(const SetVector &, SetVector &) const; 
    void computeAND(const SetVector &, SetVector &) const; 
    bool nonEmptyIntersection(const SetVector &) const; 
    imp getOR(const SetVector &) const;
    imp getAND(const SetVector &) const;
    imp getDIFF(const SetVector &) const;
private:
    imp theVector; 
};


ostream & operator<<(ostream &, const SetVector &);
istream & operator>>(istream &, SetVector &); 

#endif // !INCLUDE_SetVector_h

