#ifndef INCLUDE_UTIL_h
#define INCLUDE_UTIL_h

#include <algorithm>

double normalizemin(double ex, double ey, double mi) {
    double minnum = min(ex, ey);
    return minnum == 0.0 ? 1.0 : (mi/minnum);
}

double normalize(double ex, double ey, double mi) {
    double maxnum = max(ex, ey);
    return maxnum == 0.0 ? 1.0 : (mi/maxnum);
}

double redundify(double ex, double ey, double mi) {
    double sum = ex + ey;
    return sum == 0.0 ? 1.0 : (mi/sum);
}

#endif // !INCLUDE_UTIL_h
