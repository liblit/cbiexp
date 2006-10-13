#include <iostream>
#include <numeric>
#include <vector>
#include "NumRuns.h"
#include "SetVector.h"
#include "RunSet.h"

using namespace std;

////////////////////////////////////////////////////////////////////////
//
//  a set of run ids represented as a membership bit vector
//

RunSet::RunSet()
{
    resize(NumRuns::end);
}
