#include <iostream>
#include "Unbounded.h"
#include "tty.h"

using namespace std;


Progress::Unbounded::Unbounded(const char task[])
  : currentStep(0),
    task(task)
{
}


Progress::Unbounded::~Unbounded()
{
  tty << endl;
}


void
Progress::Unbounded::step()
{
  ++currentStep;
  tty << '\r' << task << ": " << currentStep << flush;
}
