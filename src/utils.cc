#include <cassert>
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

void print_pred_full(FILE* fp, const string &srcdir, pred_info pi)
{
    fputs("<predictor>", fp);
    print_pred_stat(fp, pi);
    print_pred_name(fp, srcdir, pi.site, pi.p);
    fputs("</predictor>", fp);
}

void print_pred_stat(FILE* fp, pred_info pi)
{
    fprintf(fp,
	    "<true success=\"%d\" failure=\"%d\"/>"
	    "<seen success=\"%d\" failure=\"%d\"/>"
	    "<scores log10-seen=\"%g\" context=\"%g\" lower-bound=\"%g\" increase=\"%g\" badness=\"%g\"/>",
	    pi.s, pi.f,
	    pi.os, pi.of,
	    log10(pi.s + pi.f), pi.ps.co, pi.ps.lb, pi.ps.in, pi.ps.fs);
}

const char* s_op[6] = { "&lt;", "&gt;=", "==", "!=", "&gt;", "&lt;=" };
const char* r_op[6] = { "&lt;", "&gt;=", "==", "!=", "&gt;", "&lt;=" };
const char* b_op[2] = { "is FALSE", "is TRUE" };
const char* g_op[4] = { "= 0", "= 1", "&gt; 1", "invalid" };

void print_pred_name(FILE* fp, const string &srcdir, int site, int p)
{
    fputs("<source predicate=\"", fp);

    switch(sites[site].scheme_code) {
    case 'S':
        fprintf(fp, "%s %s %s",
            sites[site].args[0], s_op[p], sites[site].args[1]);
        break;
    case 'R':
        fprintf(fp, "%s %s 0",
            sites[site].args[0], r_op[p]);
        break;
    case 'B':
        fprintf(fp, "%s %s",
            sites[site].args[0], b_op[p]);
        break;
    case 'G':
        fprintf(fp, "old_refcount(%s) %s",
            sites[site].args[0], g_op[p]);
        break;
    default:
        assert(0);
    }

    string collect;
    const char *filename = sites[site].file;

    if (filename[0] != '/' && !srcdir.empty() && srcdir != ".") {
	collect = srcdir;
	collect += '/';
	collect += filename;
	filename = collect.c_str();
    }

    fprintf(fp, "\" function=\"%s\" file=\"%s\" line=\"%d\"/>",
	    sites[site].fun, filename, sites[site].line);
}

void process_report(FILE* fp,
                    void (*process_s_site)(int u, int c, int x, int y, int z),
                    void (*process_r_site)(int u, int c, int x, int y, int z),
                    void (*process_b_site)(int u, int c, int x, int y),
                    void (*process_g_site)(int u, int c, int x, int y, int z, int w))
{
    int u, c, x, y, z, w;
    bool problem = false;

    while (1) {
        fscanf(fp, "%d\n", &u);
        if (feof(fp))
            break;
	if (u < num_units)
	    switch (units[u].scheme_code) {
	    case 'S':
		for (c = 0; c < units[u].num_sites; c++) {
		    fscanf(fp, "%d %d %d\n", &x, &y, &z);
		    process_s_site(u, c, x, y, z);
		}
		break;
	    case 'B':
		for (c = 0; c < units[u].num_sites; c++) {
		    fscanf(fp, "%d %d\n", &x, &y);
		    process_b_site(u, c, x, y);
		}
		break;
	    case 'R':
		for (c = 0; c < units[u].num_sites; c++) {
		    fscanf(fp, "%d %d %d\n", &x, &y, &z);
		    process_r_site(u, c, x, y, z);
		}
		break;
	    case 'G':
		for (c = 0; c < units[u].num_sites; c++) {
		    fscanf(fp, "%d %d %d %d\n", &x, &y, &z, &w);
		    process_g_site(u, c, x, y, z, w);
		}
		break;
	    default:
		fprintf(stderr, "unit %d has unknown scheme code '%c'\n",
			u, units[u].scheme_code);
		problem = true;
	    }
	else {
	    fprintf(stderr, "unit %d exceeds unit count (%d)\n", u, num_units);
	    problem = true;
	}
    }

    if (problem)
	exit(1);
}


// Local variables:
// c-file-style: "cc-mode"
// End:
