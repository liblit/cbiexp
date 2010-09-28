#include <cmath>
#include <iostream>
#include <fstream>
#include <stdio.h>

#include <list>

#include "combine.h"

#include "Confidence.h"
#include "corrective-ranking/allFailures.h"
#include "corrective-ranking/Complex.h"

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
    read_pairs();
}

void xml_conj_info(list<Complex> &c, const char *fname) {
    ofstream out(fname);
    list<Complex>::iterator iter;
    
    // Write a DTD
    out << "<?xml version=\"1.0\"?>"
        << "<conjunction-info>" << endl;
    
    for(iter = c.begin(); iter != c.end(); iter ++) {
        out << "<info index=\"" << (*iter).index << "\">" << endl;
        
        Complex conj = *iter;
        
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

void xml_conj_view(list<Complex> c, const char *fname) {
    ofstream out(fname);
    list<Complex>::iterator iter;
    
    // Write a DTD
    out << "<?xml version=\"1.0\"?>"
        << "<?xml-stylesheet type=\"text/xsl\" href=\"pred-scores.xsl\"?>"
        << "<scores>" << endl;
    
    for(iter = c.begin(); iter != c.end(); iter ++) {
        out << "<conjunction index=\"" << (*iter).index << "\" score=\"" << (*iter).getType() << (*iter).score() << "\" stat1=\"1\" stat2=\"0\"/>" << endl;
    }
    
    out << "</scores>" << endl;
}

void gen_complex() {
    initialize();

    Candidates candidates;
    candidates.load();
    candidates.sort();
    candidates.reverse();

    unsigned perfPredCount = 0;
    Candidates::iterator iter;
    for(iter = candidates.begin(); iter != candidates.end(); iter ++)
      if(iter->isPerfect())
        perfPredCount ++;
    
    FILE * fout = fopen( "complex_all.txt", "w" );
    
    std::list<Complex> result = combine1(candidates, 100, 0, fout);
    
    fprintf(fout, "Number of perfect complex predicates: %u\n", Complex::perfectCount);
    fprintf(fout, "Number of perfect single predicates: %u\n", perfPredCount);
    fclose(fout);

    xml_conj_info(result, "complex-info.xml");
    
    result.sort();
    result.reverse();
    xml_conj_view(result, "complex_hl_none.xml");
}

