#include <cassert>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "Confidence.h"
#include "utils.h"

using std::string;


pred_stat compute_pred_stat(int s, int f, int os, int of, int confidence)
{
    pred_stat p;
    int s_f = s + f;
    int os_of = os + of;

    p.fs = s_f == 0 ? 0.0 : ((double) f) / s_f;
    p.co = os_of == 0 ? 0.0 : ((double) of) / os_of;

    p.in = s == os && of == os ? 0.0 : p.fs - p.co;

    p.lb = p.in - Confidence::critical(confidence) *
      sqrt(s_f == 0 ? 0.0 : ((p.fs * (1 - p.fs)) / s_f) +
          (os_of == 0 ? 0.0 : (p.co * (1 - p.co))/ os_of));
    return p;
}

bool retain_pred(int s, int f, double lb) { return lb > 0 && s + f >= 10; }

bool retain_pred(int s, int f, int os, int of, int confidence)
{
    return retain_pred(s, f, (compute_pred_stat(s, f, os, of, confidence)).lb);
}

bool read_pred_full(FILE* fp, pred_info &pi)
{
    char scheme_code;

    const int got = fscanf(fp, "%c %d %d %lf %lf %lf %lf %d %d %d %d\n",
			   &scheme_code,
			   &pi.siteIndex,
			   &pi.predicate,
			   &pi.ps.lb, &pi.ps.in, &pi.ps.fs, &pi.ps.co,
			   &pi.s, &pi.f, &pi.os, &pi.of);

    return got == 11;
}


const string &
scheme_name(char code)
{
    switch (code) {
    case 'A': { 
	static const string name("atoms");
	return name;
    }
    case 'B': {
	static const string name("branches");
	return name;
    }
    case 'F': {
	static const string name("float-kinds");
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
    case 'Y': { 
	static const string name("yields");
	return name;
    }
    default:
	fprintf(stderr, "unrecognized scheme code: %c\n", code);
	exit(1);
    }
}

PredCoords
notP (PredCoords &p) {
  PredCoords notp(p);
  if (p.predicate % 2 == 0) {
    notp.predicate = p.predicate + 1;
  }
  else {
    notp.predicate = p.predicate - 1;
  }

  return notp;
}

// Local variables:
// c-file-style: "cc-mode"
// End:
