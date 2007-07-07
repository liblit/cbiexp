#ifndef INCLUDE_Bugs_h
#define INCLUDE_Bugs_h

#include <vector>

using namespace std;

class Bugs {
    public:
        Bugs();
        vector <int> * bugIndex();
    private:
        vector<int> * bugIds;
};

#endif // !INCLUDE_Bugs_h

