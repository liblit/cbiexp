#ifndef INCLUDE_Progress_h
#define INCLUDE_Progress_h


////////////////////////////////////////////////////////////////////////
//
//  Simple progress feedback
//


class Progress
{
public:
  Progress(const char [], unsigned);
  ~Progress();

  void step();

private:
  unsigned currentStep;
  const unsigned numSteps;
  const char * const task;
};


#endif // !INCLUDE_Progress_h
