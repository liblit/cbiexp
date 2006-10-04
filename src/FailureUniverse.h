#ifndef FAILURE_UNIVERSE_H
#define FAILURE_UNIVERSE_H

class EmptyUniverseException
{
};

class FailureUniverse
{
public:
    FailureUniverse();
    double mean(RunSet &) const;
    double covariance(RunSet &, RunSet &) const;
    double p_Xtrue_Ytrue(RunSet &, RunSet &) const;
    double p_Xtrue_Yfalse(RunSet &, RunSet &) const;
    double p_Xfalse_Yfalse(RunSet &, RunSet &) const;
    double p_Xtrue(RunSet &) const;
    double p_Xfalse(RunSet &) const;

private:
    unsigned int cardinality;
    unsigned int universeSize() const;
};

#endif // !FAILURE_UNIVERSE_H
