#include <cmath>
#include <utils.h>

float compute_lb(int s, int f, int os, int of, float conf)
{
    float fs = ((float)  f) / ( s +  f);
    float co = ((float) of) / (os + of);
    float in = fs - co;
    return in - conf * sqrt(((fs * (1 - fs)) / (f + s)) + ((co * (1 - co))/(of + os)));
}

