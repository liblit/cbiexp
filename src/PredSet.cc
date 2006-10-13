#include <iostream>
#include <numeric>
#include <vector>
#include "PredStats.h"
#include "SetVector.h"
#include "PredSet.h"

using namespace std;

PredSet::PredSet()
{
    resize(PredStats::count());
}
