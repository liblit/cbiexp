#include <iostream>
#include "Unbounded.h"
#include "enabled.h"

using namespace std;


Progress::Unbounded::Unbounded(const char task[])
  : currentStep(0),
    task(task)
{
}


Progress::Unbounded::~Unbounded()
{
  if (enabled)
    cout << endl;
}


void
Progress::Unbounded::step()
{
  if (enabled)
    {
      ++currentStep;
      cout << '\r' << task << ": " << currentStep << flush;
    }
}
