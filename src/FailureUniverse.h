#ifndef FAILURE_UNIVERSE_H
#define FAILURE_UNIVERSE_H

class EmptyUniverseException
{
};

class IsMember : public unary_function <unsigned int, bool> { 
public:
    IsMember();
    bool operator()(unsigned int) const;
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
    unsigned int begin;
    unsigned int end;
    IsMember test;
    unsigned int count() const;
    template <class Predicate> unsigned int count(RunSet &, Predicate) const;
    template <class Predicate1, class Predicate2> unsigned int count(RunSet &, Predicate1, RunSet &, Predicate2) const;
};

#endif // !FAILURE_UNIVERSE_H
