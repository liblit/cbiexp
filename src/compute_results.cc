#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iostream>
#include <vector>
#include "classify_runs.h"
#include "scaffold.h"
#include "units.h"
#include "sites.h"

using namespace std;

struct site_summary_t {
    int S[6], F[6];
    int os, of;
    bool retain[6];
    site_summary_t()
    {
	os = of = 0;
	for (int i = 0; i < 6; i++) {
	    S[i] = 0;
	    F[i] = 0;
            retain[i] = false;
	}
    }
};

vector<vector<site_summary_t> > site_summary;

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

char* experiment_name = NULL;
char* program_src_dir = NULL;
int   confidence = -1;

char* sruns_txt_file = NULL;
char* fruns_txt_file = NULL;
char* compact_report_path_fmt = NULL;
char* trace_txt_file = NULL;

char* preds_txt_file = NULL;
char* result_summary_htm_file = NULL;

float conf;

int num_s_preds = 0;
int num_r_preds = 0;
int num_b_preds = 0;

FILE* preds_txt_fp = NULL;

/****************************************************************************
 * Procedures for deciding whether to retain/discard
 * each instrumented predicate
 ***************************************************************************/

inline void inc(int r, int u, int c, int p)
{                                  
    if (is_srun[r])                
	site_summary[u][c].S[p]++; 
    else if (is_frun[r])           
	site_summary[u][c].F[p]++; 
}

inline void obs(int r, int u, int c) 
{                               
    if (is_srun[r])               
	site_summary[u][c].os++; 
    else if (is_frun[r])        
	site_summary[u][c].of++; 
}

void process_site(FILE* fp, int r, int u, int c)
{
    int x, y, z;

    switch (units[u].s[0]) {
    case 'S':
    case 'R':
	fscanf(fp, "%d %d %d\n", &x, &y, &z);
	if (x + y + z > 0) {
	    obs(r, u, c);
	    if (x > 0)
		inc(r, u, c, 0);
	    if (y + z > 0)
		inc(r, u, c, 1);
	    if (y > 0)
		inc(r, u, c, 2);
	    if (x + z > 0)
		inc(r, u, c, 3);
	    if (z > 0)
		inc(r, u, c, 4);
	    if (x + y > 0)
		inc(r, u, c, 5);
	}
	break;
    case 'B':
	fscanf(fp, "%d %d\n", &x, &y);
	if (x + y > 0) {
	    obs(r, u, c);
	    if (x > 0) inc(r, u, c, 0);
	    if (y > 0) inc(r, u, c, 1);
	}
	break;
    default:
        assert(0);
    }
}

inline void cull(int u, int c, int p)
{
    int s  = site_summary[u][c].S[p];
    int f  = site_summary[u][c].F[p];
    int os = site_summary[u][c].os;
    int of = site_summary[u][c].of;

    float fs = ((float)  f) / ( s +  f);
    float co = ((float) of) / (os + of);
    float in = fs - co;
    float lb = in - conf * sqrt(((fs * (1 - fs)) / (f + s)) + ((co * (1 - co))/(of + os)));

    if (lb > 0 && s + f >= 10) 
        site_summary[u][c].retain[p] = true;
}

void cull_preds()
{
    int u, c, p;

    for (u = 0; u < NumUnits; u++) {
	for (c = 0; c < units[u].c; c++) {
	    switch (units[u].s[0]) {
            case 'S':
                for (p = 0; p < 6; p++)
                    cull(u, c, p);
                break;
            case 'R':
                for (p = 0; p < 6; p++)
                    cull(u, c, p);
                break;
            case 'B':
                for (p = 0; p < 2; p++)
                    cull(u, c, p);
                break;
            default:
                assert(0);
            }
        }
    }
}

void cull_preds_aggressively()
{
    enum { LT, GEQ, EQ, NEQ, GT, LEQ };
    int u, c;

    for (u = 0; u < NumUnits; u++) {
	for (c = 0; c < units[u].c; c++) {
	    switch (units[u].s[0]) {
            case 'S':
            case 'R':
            {
                site_summary_t& s = site_summary[u][c];

                if (s.retain[LEQ] &&
                    s.retain[LT ] &&
                    s.retain[EQ ] &&
                    s.S[LEQ] == s.S[LT] && s.F[LEQ] == s.F[LT] &&
                    s.S[LEQ] == s.S[EQ] && s.F[LEQ] == s.F[EQ]) {
                    s.retain[LT] = s.retain[EQ] = false;
                    break;
                }             
                if (s.retain[GEQ] &&
                    s.retain[GT ] &&
                    s.retain[EQ ] &&
                    s.S[GEQ] == s.S[GT] && s.F[GEQ] == s.F[GT] &&
                    s.S[GEQ] == s.S[EQ] && s.F[GEQ] == s.F[EQ]) {
                    s.retain[GT] = s.retain[EQ] = false;
                    break;
                }
                if (s.retain[NEQ] &&
                    s.retain[LT ] &&
                    s.retain[GT ] &&
                    s.S[NEQ] == s.S[LT] && s.F[NEQ] == s.F[LT] &&
                    s.S[NEQ] == s.S[GT] && s.F[NEQ] == s.F[GT]) {
                    s.retain[LT] = s.retain[GT] = false;
                    break;
                }
                if (s.retain[LT ] &&
                    s.retain[LEQ] &&
                    s.retain[NEQ] &&
                    s.S[LT] == s.S[LEQ] && s.F[LT] == s.F[LEQ] &&
                    s.S[LT] == s.S[NEQ] && s.F[LT] == s.F[NEQ]) {
                    s.retain[LEQ] = s.retain[NEQ] = false;
                    break;
                }
                if (s.retain[GT ] &&
                    s.retain[GEQ] &&
                    s.retain[NEQ] &&
                    s.S[GT] == s.S[GEQ] && s.F[GT] == s.F[GEQ] &&
                    s.S[GT] == s.S[NEQ] && s.F[GT] == s.F[NEQ]) {
                    s.retain[GEQ] = s.retain[NEQ] = false;
                    break;
                }
                if (s.retain[EQ ] &&
                    s.retain[LEQ] &&
                    s.retain[GEQ] &&
                    s.S[EQ] == s.S[LEQ] && s.F[EQ] == s.F[LEQ] &&
                    s.S[EQ] == s.S[GEQ] && s.F[EQ] == s.F[GEQ]) {
                    s.retain[LEQ] = s.retain[GEQ] = false;
                    break;
                }
                if (s.retain[LT ] &&
                    s.retain[LEQ] &&
                    s.S[LT] == s.S[LEQ] && s.F[LT] == s.F[LEQ]) {
                    s.retain[LEQ] = false;
                    break;
                }
                if (s.retain[LT ] &&
                    s.retain[NEQ] &&
                    s.S[LT] == s.S[NEQ] && s.F[LT] == s.F[NEQ]) {
                    s.retain[NEQ] = false;
                    break;
                }
                if (s.retain[GT ] &&
                    s.retain[GEQ] &&
                    s.S[GT] == s.S[GEQ] && s.F[GT] == s.F[GEQ]) {
                    s.retain[GEQ] = false;
                    break;
                }
                if (s.retain[GT ] &&
                    s.retain[NEQ] &&
                    s.S[GT] == s.S[NEQ] && s.F[GT] == s.F[NEQ]) {
                    s.retain[NEQ] = false;
                    break;
                }
                if (s.retain[EQ ] &&
                    s.retain[LEQ] &&
                    s.S[EQ] == s.S[LEQ] && s.F[EQ] == s.F[LEQ]) {
                    s.retain[LEQ] = false;
                    break;
                }
                if (s.retain[EQ ] &&
                    s.retain[GEQ] &&
                    s.S[EQ] == s.S[GEQ] && s.F[EQ] == s.F[GEQ]) {
                    s.retain[GEQ] = false;
                    break;
                }
                break;
            }
            case 'B':
                break;
            default: assert(0);
            }
        }
    }
}

/****************************************************************************
 * Procedures for printing (1) result summary and (2) retained predicates
 ***************************************************************************/

void print_result_summary()
{
    time_t t;
    time(&t);

    FILE* fp = fopen(result_summary_htm_file, "w");
    assert(fp);

    fputs("<table>\n<tr>\n"
	  "<td align=middle><font size=\"+2\">Cooperative Bug Isolation Report</font></td>\n"
	  "<td rowSpan=2 align=right>"
	  "<a  href=\"http://www.cs.berkeley.edu/~liblit/sampler/\">"
          "<img src=\"http://www.cs.berkeley.edu/~liblit/sampler/logo.png\" style=\"border-style: none\"></a></td>\n</tr>\n"
	  "</table>\n<br>\n",
	  fp);
    fprintf(fp, "Experiment name: %s\n<p>\n", experiment_name);
    fprintf(fp, "Generated on %s<p>\n", ctime(&t));
    fprintf(fp, "# runs: %d [successful: %d failing: %d discarded: %d]\n<p>\n",
	num_runs, num_sruns, num_fruns, num_runs - (num_sruns + num_fruns));
    fprintf(fp, "# predicates instrumented: %d [branch: %d return: %d scalar: %d]\n<p>\n",
        NumBPreds + NumRPreds + NumSPreds, NumBPreds, NumRPreds, NumSPreds);
    fprintf(fp, "# predicates retained:     %d [branch: %d return: %d scalar: %d]\n<p>\n",
	num_b_preds + num_r_preds + num_s_preds, num_b_preds, num_r_preds, num_s_preds);
    fprintf(fp, "Confidence: %d%%\n<p>\n", confidence);

    fclose(fp);
}

inline void print_pred_info(FILE* fp, int u, int c, int p)
{
    int s  = site_summary[u][c].S[p];
    int f  = site_summary[u][c].F[p];
    int os = site_summary[u][c].os;
    int of = site_summary[u][c].of;

    float fs = ((float)  f) / ( s +  f);
    float co = ((float) of) / (os + of);
    float in = fs - co;
    float lb = in - conf * sqrt(((fs * (1 - fs)) / (f + s)) + ((co * (1 - co))/(of + os)));

    fprintf(fp, "%c %d %d %d %.2f %.2f %.2f %.2f %d %d %d %d ",
	units[u].s[0], u, c, p, lb, in, fs, co, s, f, os, of);
}

inline void print_site_info(FILE* fp, int i)
{
    fprintf(fp, "</td><td>%s</td><td>%s:%d\n", sites[i].fun, sites[i].file, sites[i].line);
}

inline void print_s_pred_name(FILE* fp, int i, char op, bool is_neg)
{
    if (is_neg) fputs("!(", fp);
    fprintf(fp, "%s %c %s", sites[i].args[0], op, sites[i].args[1]);
    if (is_neg) fputc(')', fp);
}

inline void print_r_pred_name(FILE* fp, int i, const char* op, bool is_neg)
{
    if (is_neg) fputs("!(", fp);
    fprintf(fp, "%s %s", sites[i].args[0], op);
    if (is_neg) fputc(')',  fp);
}

inline void print_b_pred_name(FILE* fp, int i, const char* op)
{
    fprintf(fp, "%s %s", sites[i].args[0], op);
}

void print_retained_preds()
{
    const char* branch_op[2] = { "is FALSE", "is TRUE" };
    const char* return_op[3] = { "< 0", "= 0", "> 0" };
    const char  scalar_op[3] = { '<', '=', '>' };

    int u, c, p, i = 0;

    FILE* fp = fopen(preds_txt_file, "w");
    assert(fp);

    for (u = 0; u < NumUnits; u++) {
	for (c = 0; c < units[u].c; c++, i++) {
            switch (units[u].s[0]) {
            case 'S':
	        for (p = 0; p < 6; p++)
	            if (site_summary[u][c].retain[p]) {
		        num_s_preds++;
		        print_pred_info(fp, u, c, p);
		        print_s_pred_name(fp, i, scalar_op[p / 2], p % 2 ? true : false);
                        print_site_info(fp, i);
	            }
	        break;
            case 'R':
	        for (p = 0; p < 6; p++)
	            if (site_summary[u][c].retain[p]) {
		        num_r_preds++;
	                print_pred_info(fp, u, c, p);
		        print_r_pred_name(fp, i, return_op[p / 2], p % 2 ? true : false);
                        print_site_info(fp, i);
                    }
	        break;
            case 'B':
	        for (p = 0; p < 2; p++)
	            if (site_summary[u][c].retain[p]) {
	                num_b_preds++;
	                print_pred_info(fp, u, c, p);
	                print_b_pred_name(fp, i, branch_op[p]);
                        print_site_info(fp, i);
                    }
	        break;
            default:
                assert(0);
            }
        }
    }

    fclose(fp);
}

/****************************************************************************
 * Processing command line options
 ***************************************************************************/

void process_cmdline(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
	if (!strcmp(argv[i], "-e")) {
	    i++;
	    experiment_name = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-d")) {
	    i++;
	    program_src_dir = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-c")) {
	    i++;
	    confidence = atoi(argv[i]);
	    assert(confidence >= 90 && confidence < 100);
	    continue;
	}
	if (!strcmp(argv[i], "-s")) {
	    i++;
	    sruns_txt_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-f")) {
	    i++;
	    fruns_txt_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-cr")) {
	    i++;
	    compact_report_path_fmt = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-p")) {
	    i++;
	    preds_txt_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-r")) {
	    i++;
	    result_summary_htm_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-h")) {
	    puts("Usage: compute-results <options>\n"
                 "    -e  <experiment-name>\n"
                 "    -d  <program-src-dir>\n"
                 "    -c  <confidence>\n"
                 "(r) -s  <sruns-txt-file>\n"
                 "(r) -f  <fruns-txt-file>\n"
                 "(r) -cr <compact-report-path-fmt>\n"
                 "(w) -p  <preds-txt-file>\n"
                 "(w) -r  <result-summary-htm-file>\n");
	    exit(0);
	}
	printf("Illegal option: %s\n", argv[i]);
	exit(1);
    }

    if (!experiment_name ||
        !program_src_dir ||
        confidence == -1 ||
        !sruns_txt_file ||
        !fruns_txt_file ||
        !compact_report_path_fmt ||
	!preds_txt_file ||
        !result_summary_htm_file) {
	puts("Incorrect usage; try -h");
	exit(1);
    }
}

/****************************************************************************
 * MAIN
 ***************************************************************************/

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);

    classify_runs(sruns_txt_file, fruns_txt_file);

    conf = conf_map[confidence - 90];

    site_summary.resize(NumUnits);
    for (int u = 0; u < NumUnits; u++) site_summary[u].resize(units[u].c);

    scaffold(compact_report_path_fmt, process_site);

    cull_preds();
    cull_preds_aggressively();

    print_retained_preds();
    print_result_summary();

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
