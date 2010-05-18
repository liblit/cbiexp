#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include "Bugs.h"


using namespace std;

Bugs::Bugs() {
    char filename[] = "bugs.txt";
    bugIds = new vector <int> ();
    ifstream bugs(filename);
    if(!bugs) {
        const int code = errno;
        cerr << "cannot read " << filename << ": " << strerror(code) << '\n';
        exit(code || 1);
    }

    int bugId;
    string s;
    while(bugs >> bugId >> s) {
        bugIds->push_back(bugId);
    }
    bugs.close();
}

vector<int> * Bugs::bugIndex() {
    return bugIds; 
}
