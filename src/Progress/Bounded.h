#ifndef INCLUDE_Progress_Bounded_h
#define INCLUDE_Progress_Bounded_h


////////////////////////////////////////////////////////////////////////
//
//  Progress feedback with a known number of total steps
//


namespace Progress
{
  class Bounded
  {
  public:
    Bounded(const char [], unsigned);
    ~Bounded();

    void step();
    void stepTo(unsigned);

  private:
    void message() const;

    unsigned currentStep;
    const unsigned numSteps;
    const char * const task;
  };
}


#endif // !INCLUDE_Progress_Bounded_h
