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
    {
      if (currentStep < numSteps)
	stepTo(numSteps);
      cout << endl;
    }
}


void
Progress::Bounded::step()
{
  if (enabled)
    {
      ++currentStep;
      message();
    }
}


void
Progress::Bounded::stepTo(unsigned value)
{
  if (enabled)
    {
      currentStep = value;
      message();
    }
}


void
Progress::Bounded::message() const
{
  cout << '\r' << task << ": " << currentStep << '/' << numSteps << ' ' << (100 * currentStep / numSteps) << '%' << flush;
}
