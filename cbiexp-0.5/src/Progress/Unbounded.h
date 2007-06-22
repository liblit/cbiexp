#ifndef INCLUDE_Progress_Unbounded_h
#define INCLUDE_Progress_Unbounded_h


////////////////////////////////////////////////////////////////////////
//
//  Progress feedback with an unknown number of total steps
//


namespace Progress
{
  class Unbounded
  {
  public:
    Unbounded(const char []);
    ~Unbounded();

    void step();

  private:
    unsigned currentStep;
    const char * const task;
  };
}


#endif // !INCLUDE_Progress_Unbounded_h
