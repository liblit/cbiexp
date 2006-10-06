#ifndef INCLUDE_RunSet_h
#define INCLUDE_RunSet_h

#include <iostream>
#include <vector>

using namespace std;


class RunSet : private vector<bool>
{
public:
    RunSet();

    void insert(unsigned);
    bool find(unsigned) const;
    size_t setSize() const;

    void print(ostream &) const;
    size_t intersectionSize(const RunSet &) const;


};

ostream & operator<<(ostream &, const RunSet &);
istream & operator>>(istream &, RunSet &); 

#endif // !INCLUDE_RunSet_h

