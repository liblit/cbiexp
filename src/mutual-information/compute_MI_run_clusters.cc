#include <argp.h>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <iterator>
#include "../RunsDirectory.h"
#include "../NumRuns.h"
#include "../Progress/Bounded.h"
#include "FailureUniverse.h"
#include "../PredStats.h"

using namespace std;

#ifdef HAVE_ARGP_H

static void process_cmdline(int argc, char **argv)
{
    static const argp_child children[] = {
        { &RunsDirectory::argp, 0, 0, 0 }, 
        { &NumRuns::argp, 0, 0, 0 },
        { 0, 0, 0, 0 }
    };

    static const argp argp = {
        0, 0, 0, 0, children, 0, 0
    };

    argp_parse(&argp, argc, argv, 0, 0, 0); 
}

#else // !HAVE_ARGP_H

inline void process_cmdline(int, char *[]) { }

#endif // HAVE_ARGP_H

/*****************************************************************************
* Set union, where set inclusion is represented by true at the 
* appropriate index in a vector<bool>.
****************************************************************************/
class Union : public binary_function <FRunSet *, FRunSet *, FRunSet *> {
public:
    FRunSet * operator()(const FRunSet * first, const FRunSet * second) const {
        FRunSet theUnion = FailureUniverse::getUniverse().makeFRunSet(); 
        FailureUniverse::getUniverse().computeUnion(*first, *second, theUnion); 
        delete first;
        delete second;
        return new FRunSet(theUnion);
    }
};

/***************************************** *************************************
* We don't need to find the set which is the intersection; we just need to know
* whether it is non-empty.
******************************************************************************/
class NonEmptyIntersection : public binary_function <FRunSet *, FRunSet *, bool> {
public:
    bool operator()(const FRunSet * first, const FRunSet * second) const {
        return FailureUniverse::getUniverse().nonEmptyIntersection(*first, *second);
    }
};

/******************************************************************************
* Take the head of the list. If any element in the tail of the list has a 
* non-empty intersection with the head remove it from the tail. Return the
* union of the head of the list and all removed elements. 
******************************************************************************/
FRunSet
coalesce(list <FRunSet *> & theList) 
{
    FRunSet * head = theList.front();
    theList.pop_front();
    list <FRunSet *>::iterator keep = partition(theList.begin(), theList.end(), bind1st(NonEmptyIntersection(), head));
    FRunSet * theUnion = accumulate(theList.begin(), keep, head, Union());
    theList.erase(theList.begin(), keep);
    FRunSet result(*theUnion);
    delete theUnion;
    return result;
}

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);
    ios::sync_with_stdio(false);

    /**************************************************************************
    * Read in the normalized mutual info between a run and all other runs
    * Store RunSets if the normalized mutual info is greater than threshold 
    **************************************************************************/
    vector < FRunSet * > sets; 
    ifstream run_MI("run_run_MI_normalized.txt");
    const unsigned int numRuns = FailureUniverse::getUniverse().cardinality(); 
    Progress::Bounded reading("calculating runs sets", numRuns);
    for(unsigned int i = 0; i < numRuns; i++) {
        reading.step();
        string line;
        getline(run_MI, line);
        istringstream parse(line);
        vector <bool> current;
        transform(istream_iterator<double>(parse), istream_iterator<double>(), back_inserter(current), bind2nd(greater<double>(),0.9));
        assert(current.size() == numRuns);
        int cardinality = count(current.begin(), current.end(), true);
        assert(cardinality > 0);
        if(cardinality > 1) {
            FRunSet temp = FailureUniverse::getUniverse().makeFRunSet();
            vector <unsigned int> indices = FailureUniverse::getUniverse().getIndices();
            for(unsigned int j = 0; j < numRuns; j++) {
                if(current[j]) temp.set(indices[j]); 
            }
            sets.push_back(new FRunSet(temp));
        }
    }
    assert(run_MI.peek() == EOF);
    run_MI.close();

    /**************************************************************************
    * Having found these sets we need to coalesce them. If two sets have a 
    * non-empty intersection we replace them with a single set which is the
    * union of the intersecting sets.
    **************************************************************************/
    ofstream out("run_sets.txt");
    Progress::Bounded coalescing("coalescing sets", sets.size()); 
    list <FRunSet * > set_list(sets.begin(), sets.end()); 
    while(!set_list.empty()) {
        coalescing.step();
        out << coalesce(set_list);
        out << "\n";
    }
    out.close();

}
