#include <cmath>
#include <utils.h>

const float conf_map[10] = {
    1.645,  // 90%
    0,
    0,
    0,
    0,
    1.96,   // 95%, default
    2.05,   // 96%
    0,
    2.33,   // 98%
    2.58    // 99%
};

float compute_lb(int s, int f, int os, int of, int conf)
{
#ifdef LIB_DEBUG
    assert(conf >= 90 && conf < 100);
#endif
    float fs = ((float)  f) / ( s +  f);
    float co = ((float) of) / (os + of);
    float in = fs - co;
    return in - conf_map[conf - 90] * sqrt(((fs * (1 - fs)) / (f + s)) + ((co * (1 - co))/(of + os)));
}

bool retain_pred(int s, int f, float lb)
{
    return lb > 0 && s + f >= 10;
}

bool retain_pred(int s, int f, int os, int of, int conf)
{
    return retain_pred(s, f, compute_lb(s, f, os, of, conf));
}

