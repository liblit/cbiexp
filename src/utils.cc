#include <cassert>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "sites.h"
#include "units.h"
#include "utils.h"

using std::string;


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

pred_stat compute_pred_stat(int s, int f, int os, int of, int confidence)
{
    pred_stat p;
    p.fs = ((float)  f) / ( s +  f);
    p.co = ((float) of) / (os + of);
    p.in = p.fs - p.co;
    p.lb = p.in - conf_map[confidence - 90] * sqrt(((p.fs * (1 - p.fs)) / (f + s)) + ((p.co * (1 - p.co))/(of + os)));
    return p;
}

bool retain_pred(int s, int f, float lb) { return lb > 0 && s + f >= 10; }

bool retain_pred(int s, int f, int os, int of, int confidence)
{
    return retain_pred(s, f, (compute_pred_stat(s, f, os, of, confidence)).lb);
}

bool read_pred_full(FILE* fp, pred_info &pi)
{
    char scheme_code;

    const int got = fscanf(fp, "%c %d %d %d %d %f %f %f %f %d %d %d %d\n",
			   &scheme_code,
			   &pi.u, &pi.c, &pi.p, &pi.site,
			   &pi.ps.lb, &pi.ps.in, &pi.ps.fs, &pi.ps.co,
			   &pi.s, &pi.f, &pi.os, &pi.of);

    if (got == 13) {
	assert(scheme_code == sites[pi.site].scheme_code);
	return true;
    } else
	return false;
}


const string &
scheme_name(char code)
{
    switch (code) {
    case 'B': {
	static const string name("branches");
	return name;
    }
    case 'G': {
	static const string name("g-object-unref");
	return name;
    }
    case 'R': {
	static const string name("returns");
	return name;
    }
    case 'S': {
	static const string name("scalar-pairs");
	return name;
    }
    default:
	fprintf(stderr, "unrecognized scheme code: %c\n", code);
	exit(1);
    }
}

// Local variables:
// c-file-style: "cc-mode"
// End:
