#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "PredUniverse.h"
#include "../ViewPrinter.h"
#include "../PredStats.h"
#include "../Progress/Unbounded.h"

using namespace std;

int main()
{
    vector <PSet *> sets;
    ifstream pred_sets("pred_sets.txt");
    Progress::Unbounded reading("reading predicate sets");
    while(pred_sets.peek() != EOF) {
        reading.step();
        string line;
        getline(pred_sets, line);
        istringstream parse(line);
        PSet current = PredUniverse::getUniverse().makePredSet();
        parse >> current;
        sets.push_back(new PSet(current));
    }
    pred_sets.close();

    const unsigned numPreds = PredStats::count();
    for(unsigned int i = 0; i < sets.size(); i++) {
        char filename[128];
        sprintf(filename, "set%u.xml", i);
        ViewPrinter out("view.xsl", "view", filename); 
        for(unsigned int j = 0; j < numPreds; j++) {
            if(sets[i]->test(j)) {
                out << "<predictor index=\"" << j + 1 << "\">" 
                    << "</predictor>";
            }
        }
    }
}
