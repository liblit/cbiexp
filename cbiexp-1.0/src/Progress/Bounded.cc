#include <iostream>
#include "Bounded.h"
#include "tty.h"

using namespace std;


Progress::Bounded::Bounded(const char task[], unsigned numSteps)
  : currentStep(0),
    numSteps(numSteps),
    task(task)
{
}


Progress::Bounded::~Bounded()
{
  if (currentStep < numSteps)
    stepTo(numSteps);
  tty << endl;
}


void
Progress::Bounded::step()
{
  ++currentStep;
  message();
}


void
Progress::Bounded::stepTo(unsigned value)
{
  currentStep = value;
  message();
}


void
Progress::Bounded::message() const
{
  tty << '\r' << task << ": " << currentStep << '/' << numSteps << ' ' << (100 * currentStep / numSteps) << '%' << flush;
}
