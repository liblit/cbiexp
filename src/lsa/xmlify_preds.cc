#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "../ViewPrinter.h"
#include "../PredStats.h"
#include "../Progress/Unbounded.h"

using namespace std;

int main()
{
    const unsigned numPreds = PredStats::count();
    char* filename = "preds.xml";
    ViewPrinter out("view.xsl", "view", filename);
    for(unsigned int j = 0; j < numPreds; j++) {
        out << "<predictor index=\"" << j + 1 << "\">" << "</predictor>";
    }
}
