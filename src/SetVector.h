#ifndef INCLUDE_SetVector_h
#define INCLUDE_SetVector_h

#include <iostream>
#include <vector>

using namespace std;

class SetVector : private vector<bool>
{
public:
    void initialize(unsigned int); 
    void initialize(const vector<bool> &);
    void initialize(vector<bool> &);
    void insert(unsigned);
    bool find(unsigned) const;
    size_t setSize() const;
    size_t numEntries() const;

    void print(ostream &) const;
    void load(istream &);
    size_t intersectionSize(const SetVector &) const;
    friend bool nonEmptyIntersection(const SetVector &, const SetVector &); 
    friend SetVector calc_union(const SetVector &, const SetVector &); 
    friend void calc_union(const SetVector &, const SetVector &, SetVector &); 
};


ostream & operator<<(ostream &, const SetVector &);
istream & operator>>(istream &, SetVector &); 

#endif // !INCLUDE_SetVector_h

