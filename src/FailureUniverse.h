#ifndef FAILURE_UNIVERSE_H
#define FAILURE_UNIVERSE_H

class EmptyUniverseException
{
};

class FailureUniverse
{
public:
    FailureUniverse();
    unsigned int cardinality;
    double p_Xtrue_Ytrue(RunSet &, RunSet &);
    double p_Xtrue_Yfalse(RunSet &, RunSet &);
    double p_Xfalse_Yfalse(RunSet &, RunSet &);
    double p_Xtrue(RunSet &);
    double p_Xfalse(RunSet &);

private:
    unsigned int universeSize();
};

#endif // !FAILURE_UNIVERSE_H
