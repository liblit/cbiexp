#include <iostream>
#include <unistd.h>
#include "Progress.h"

using namespace std;


static const bool enabled = isatty(STDOUT_FILENO) && getenv("TERM");


Progress::Progress(const char task[], unsigned numSteps)
  : currentStep(0),
    numSteps(numSteps),
    task(task)
{
}


Progress::~Progress()
{
  if (enabled)
    cout << '\n';
}


void
Progress::step()
{
  if (enabled)
    {
      ++currentStep;
      cout << '\r' << task << ": " << currentStep << '/' << numSteps << ' ' << (100 * currentStep / numSteps) << '%' << flush;
    }
}
