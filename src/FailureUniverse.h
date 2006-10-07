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
    double mean(const RunSet &) const;
    double covariance(const RunSet &, const RunSet &) const;
    unsigned int c_Xtrue_Ytrue(const RunSet &, const RunSet &) const;
    double p_Xtrue_Ytrue(const RunSet &, const RunSet &) const;
    double p_Xtrue_Yfalse(const RunSet &, const RunSet &) const;
    double p_Xfalse_Yfalse(const RunSet &, const RunSet &) const;
    double p_Xtrue(const RunSet &) const;
    double p_Xfalse(const RunSet &) const;

private:
    unsigned int cardinality;
    unsigned int begin;
    unsigned int end;
    IsMember test;
    unsigned int count() const;
    template <class Predicate> unsigned int count(const RunSet &, const Predicate) const;
    template <class Predicate1, class Predicate2> unsigned int count(const RunSet &, const Predicate1, const RunSet &, const Predicate2) const;
};

#endif // !FAILURE_UNIVERSE_H
