#include <iostream>
#include "Bounded.h"
#include "enabled.h"

using namespace std;


Progress::Bounded::Bounded(const char task[], unsigned numSteps)
  : currentStep(0),
    numSteps(numSteps),
    task(task)
{
}


Progress::Bounded::~Bounded()
{
  if (enabled)
    cout << endl;
}


void
Progress::Bounded::step()
{
  if (enabled)
    {
      ++currentStep;
      cout << '\r' << task << ": " << currentStep << '/' << numSteps << ' ' << (100 * currentStep / numSteps) << '%' << flush;
    }
}
