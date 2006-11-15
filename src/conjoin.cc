#include <cmath>
#include <iostream>
#include <fstream>

#include <list>

#include "conjoin.h"

#include "Confidence.h"
#include "corrective-ranking/allFailures.h"
#include "corrective-ranking/Conjunction.h"

using namespace std;


void
RunSet::dilute(const Predicate &, const RunSet &winner) {
    assert(size() == winner.size());    
    // Nothing
}

void initialize() {
    ifstream failuresFile("f.runs");
    assert(failuresFile);
    failuresFile >> allFailures;
    
    Confidence::level = 0;
}

void xml_conj_info(list<Conjunction> &c, char *fname) {
    ofstream out(fname);
    list<Conjunction>::iterator iter;
    
    // Write a DTD
    out << "<?xml version=\"1.0\"?>"
        << "<conjunction-info>" << endl;
    
//     unsigned i;
    for(iter = c.begin(); iter != c.end(); iter ++) {
        out << "<info index=\"" << (*iter).index << "\">" << endl;
        
        Conjunction conj = *iter;
//         (*iter).index = i;
        
        vector<unsigned> preds = conj.getPredicateList();
        for(vector<unsigned>::iterator p = preds.begin(); p != preds.end(); p ++) {
           out << "<pred index=\"" << *p + 1<< "\"/>" << endl;
        }
        
        // bug-o-meter
        out << conj.bugometerXML()
            << "</info>" << endl;
    }
    
    out << "</conjunction-info>" << endl;
    out.close();
}

void xml_conj_view(list<Conjunction> c, char *fname) {
    ofstream out(fname);
    list<Conjunction>::iterator iter;
    
    // Write a DTD
    out << "<?xml version=\"1.0\"?>"
        << "<?xml-stylesheet type=\"text/xsl\" href=\"pred-scores.xsl\"?>"
        << "<scores>" << endl;
    
    for(iter = c.begin(); iter != c.end(); iter ++) {
        out << "<conjunction index=\"" << (*iter).index << "\" score=\"" << (*iter).score() << "\" stat1=\"1\" stat2=\"0\"/>" << endl;
    }
    
    out << "</scores>" << endl;
}

void gen_conjunctions() {
    initialize();

    Candidates candidates;
    candidates.load();
    candidates.sort();
    candidates.reverse();
    
    std::list<Conjunction> result = conjoin(candidates, 100);
    
    xml_conj_info(result, "conjunction-info.xml");
    
    result.sort();
    result.reverse();
    xml_conj_view(result, "conjunction_hl_none.xml");
    
    cout << "Number of interesting conjunctions: " << result.size() << endl;
    cout << "(Max possible: " << candidates.size() * candidates.size() / 2 << ")\n";
}

