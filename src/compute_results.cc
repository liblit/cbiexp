#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cctype>
#include <vector>
#include "classify_runs.h"
#include "units.h"
#include "sites.h"
#include "utils.h"

using namespace std;

enum { LT, GEQ, EQ, NEQ, GT, LEQ };

struct site_info_t {
    int S[6], F[6];
    int os, of;
    bool retain[6];
    int lb[6]; 
    site_info_t()
    {
	os = of = 0;
	for (int i = 0; i < 6; i++) {
	    S[i] = 0;
	    F[i] = 0;
            retain[i] = false;
            lb[i] = -1;
	}
    }
};

vector<vector<site_info_t> > site_info;

char* experiment_name = NULL;
char* program_src_dir = NULL;
int   confidence = -1;

char* sruns_txt_file = NULL;
char* fruns_txt_file = NULL;
char* compact_report_path_fmt = NULL;

char* preds_txt_file = NULL;
char* prefix = NULL;
char* result_summary_xml_file = NULL;

int num_s_preds = 0;
int num_r_preds = 0;
int num_b_preds = 0;
int num_g_preds = 0;

FILE* preds_txt_fp = NULL;

/****************************************************************************
 * Procedures for printing (1) result summary and (2) retained predicates
 ***************************************************************************/

static void print_scheme_summary(FILE *fp,
				 const char name[], char code,
				 int total, int retain)
{
    fprintf(fp, "<scheme name=\"%s\" code=\"%c\" total=\"%d\" retain=\"%d\"/>",
	    name, code, total, retain);
}

void print_result_summary()
{
    time_t t;
    time(&t);

    FILE* fp = fopen(result_summary_xml_file, "w");
    assert(fp);

    fprintf(fp,
	    "<?xml version=\"1.0\"?>"
	    "<?xml-stylesheet type=\"text/xsl\" href=\"summary.xsl\"?>"
	    "<!DOCTYPE experiment SYSTEM \"summary.dtd\">"
	    "<experiment title=\"%s\" date=\"%s\">"
	    "<runs success=\"%d\" failure=\"%d\" ignore=\"%d\"/>"
	    "<analysis confidence=\"%d\" prefix=\"%s\"/>"
	    "<schemes>",
	    experiment_name, ctime(&t),
	    num_sruns, num_fruns, num_runs - (num_sruns + num_fruns),
	    confidence, prefix);

    print_scheme_summary(fp, "branches", 'B', NumBPreds, num_b_preds);
    print_scheme_summary(fp, "returns", 'R', NumRPreds, num_r_preds);
    print_scheme_summary(fp, "scalar-pairs", 'S', NumSPreds, num_s_preds);
    print_scheme_summary(fp, "g-object-unref", 'G', NumGPreds, num_g_preds);

    fputs("</schemes></experiment>\n", fp);

    fclose(fp);
}

inline void print_pred(FILE* fp, int u, int c, int p, int site)
{
    int s  = site_info[u][c].S[p];
    int f  = site_info[u][c].F[p];
    int os = site_info[u][c].os;
    int of = site_info[u][c].of;

    pred_stat ps = compute_pred_stat(s, f, os, of, confidence);

    assert(units[u].scheme_code == sites[site].scheme_code);

    fprintf(fp, "%c %d %d %d %d %g %g %g %g %d %d %d %d\n",
	units[u].scheme_code,
        u, c, p, site, 
        ps.lb, ps.in, ps.fs, ps.co,
        s, f, os, of);
}

void print_retained_preds()
{
    int u, c, p, site = 0;

    FILE* fp = fopen(preds_txt_file, "w");
    assert(fp);

    for (u = 0; u < num_units; u++) {
	for (c = 0; c < units[u].num_sites; c++, site++) {
            switch (units[u].scheme_code) {
            case 'S':
	        for (p = 0; p < 6; p++)
	            if (site_info[u][c].retain[p]) {
		        num_s_preds++;
		        print_pred(fp, u, c, p, site);
	            }
	        break;
            case 'R':
	        for (p = 0; p < 6; p++)
	            if (site_info[u][c].retain[p]) {
		        num_r_preds++;
	                print_pred(fp, u, c, p, site);
                    }
	        break;
            case 'B':
	        for (p = 0; p < 2; p++)
	            if (site_info[u][c].retain[p]) {
	                num_b_preds++;
	                print_pred(fp, u, c, p, site);
                    }
	        break;
            case 'G':
	        for (p = 0; p < 4; p++)
	            if (site_info[u][c].retain[p]) {
	                num_g_preds++;
	                print_pred(fp, u, c, p, site);
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
 * Procedures for deciding whether to retain/discard
 * each instrumented predicate
 ***************************************************************************/

inline void inc(int r, int u, int c, int p)
{                                  
    if (is_srun[r])                
	site_info[u][c].S[p]++; 
    else if (is_frun[r])           
	site_info[u][c].F[p]++; 
}

inline void obs(int r, int u, int c) 
{                               
    if (is_srun[r])               
	site_info[u][c].os++; 
    else if (is_frun[r])        
	site_info[u][c].of++; 
}

int cur_run;

void process_s_site(int u, int c, int x, int y, int z)
{
	if (x + y + z > 0) {
		obs(cur_run, u, c);
		if (x > 0)
			inc(cur_run, u, c, 0);
		if (y + z > 0)
			inc(cur_run, u, c, 1);
		if (y > 0)
			inc(cur_run, u, c, 2);
		if (x + z > 0)
			inc(cur_run, u, c, 3);
		if (z > 0)
			inc(cur_run, u, c, 4);
		if (x + y > 0)
			inc(cur_run, u, c, 5);
	}
}

void process_b_site(int u, int c, int x, int y)
{
	if (x + y > 0) {
		obs(cur_run, u, c);
		if (x > 0) inc(cur_run, u, c, 0);
		if (y > 0) inc(cur_run, u, c, 1);
	}
}

void process_g_site(int u, int c, int x, int y, int z, int w)
{
	if (x + y + z + w > 0) {
		obs(cur_run, u, c);
		if (x > 0) inc(cur_run, u, c, 0);
		if (y > 0) inc(cur_run, u, c, 1);
		if (z > 0) inc(cur_run, u, c, 2);
		if (w > 0) inc(cur_run, u, c, 3);
	}
}

inline void cull(int u, int c, int p)
{
    int s = site_info[u][c].S[p];
    int f = site_info[u][c].F[p];
    pred_stat ps = compute_pred_stat(s, f, site_info[u][c].os, site_info[u][c].of, confidence);

    if (retain_pred(s, f, ps.lb)) {
        site_info[u][c].retain[p] = true;
        site_info[u][c].lb[p] = (int) rint(ps.lb * 100);
    }
}

void cull_preds()
{
    int u, c, p;

    for (u = 0; u < num_units; u++) {
	for (c = 0; c < units[u].num_sites; c++) {
	    switch (units[u].scheme_code) {
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
            case 'G':
                for (p = 0; p < 4; p++)
                    cull(u, c, p);
                break;
            default:
                assert(0);
            }
        }
    }
}

inline bool have_equal_increase(int u1, int c1, int p1, int u2, int c2, int p2)
{
    return site_info[u1][c1].lb[p1] == site_info[u2][c2].lb[p2];
}

void cull_preds_aggressively1()
{
    int u, c;

    for (u = 0; u < num_units; u++) {
	for (c = 0; c < units[u].num_sites; c++) {
	    switch (units[u].scheme_code) {
            case 'S':
            case 'R':
            {
                site_info_t& s = site_info[u][c];

                if (s.retain[LEQ] &&
                    s.retain[LT ] &&
                    s.retain[EQ ] &&
                    have_equal_increase(u, c, LT, u, c, LEQ) &&
                    have_equal_increase(u, c, EQ, u, c, LEQ)) {
                    s.retain[LT] = s.retain[EQ] = false;
                    break;
                }             
                if (s.retain[GEQ] &&
                    s.retain[GT ] &&
                    s.retain[EQ ] &&
                    have_equal_increase(u, c, GT, u, c, GEQ) &&
                    have_equal_increase(u, c, EQ, u, c, GEQ)) {
                    s.retain[GT] = s.retain[EQ] = false;
                    break;
                }
                if (s.retain[NEQ] &&
                    s.retain[LT ] &&
                    s.retain[GT ] &&
                    have_equal_increase(u, c, LT, u, c, NEQ) &&
                    have_equal_increase(u, c, GT, u, c, NEQ)) {
                    s.retain[LT] = s.retain[GT] = false;
                    break;
                }
                if (s.retain[LT ] &&
                    s.retain[LEQ] &&
                    s.retain[NEQ] &&
                    have_equal_increase(u, c, LEQ, u, c, LT) &&
                    have_equal_increase(u, c, NEQ, u, c, LT)) {
                    s.retain[LEQ] = s.retain[NEQ] = false;
                    break;
                }
                if (s.retain[GT ] &&
                    s.retain[GEQ] &&
                    s.retain[NEQ] &&
                    have_equal_increase(u, c, GEQ, u, c, GT) &&
                    have_equal_increase(u, c, NEQ, u, c, GT)) {
                    s.retain[GEQ] = s.retain[NEQ] = false;
                    break;
                }
                if (s.retain[EQ ] &&
                    s.retain[LEQ] &&
                    s.retain[GEQ] &&
                    have_equal_increase(u, c, LEQ, u, c, EQ) &&
                    have_equal_increase(u, c, GEQ, u, c, EQ)) {
                    s.retain[LEQ] = s.retain[GEQ] = false;
                    break;
                }
                if (s.retain[LT ] &&
                    s.retain[LEQ] &&
                    have_equal_increase(u, c, LT, u, c, LEQ)) {
                    s.retain[LEQ] = false;
                    break;
                }
                if (s.retain[LT ] &&
                    s.retain[NEQ] &&
                    have_equal_increase(u, c, LT, u, c, NEQ)) {
                    s.retain[NEQ] = false;
                    break;
                }
                if (s.retain[GT ] &&
                    s.retain[GEQ] &&
                    have_equal_increase(u, c, GT, u, c, GEQ)) {
                    s.retain[GEQ] = false;
                    break;
                }
                if (s.retain[GT ] &&
                    s.retain[NEQ] &&
                    have_equal_increase(u, c, GT, u, c, NEQ)) {
                    s.retain[NEQ] = false;
                    break;
                }
                if (s.retain[EQ ] &&
                    s.retain[LEQ] &&
                    have_equal_increase(u, c, EQ, u, c, LEQ)) {
                    s.retain[LEQ] = false;
                    break;
                }
                if (s.retain[EQ ] &&
                    s.retain[GEQ] &&
                    have_equal_increase(u, c, EQ, u, c, GEQ)) {
                    s.retain[GEQ] = false;
                    break;
                }
                break;
            }
            case 'B':
            case 'G':
                break;
            default:
                assert(0);
            }
        }
    }
}

inline bool is_eligible(int u, int c, int p, int s)
{
    return site_info[u][c].retain[p] && 
           (isdigit(sites[s + c].args[1][0]) || sites[s + c].args[1][0] != '-'); 
}

void checkG(int u, int c, int p, int s)
{
    for (int d = 0; d < units[u].num_sites; d++) {
        if (c != d &&
            sites[s + c].line == sites[s + d].line &&
            strcmp(sites[s + c].args[0], sites[s + d].args[0]) == 0 &&
            atoi(sites[s + c].args[1]) < atoi(sites[s + d].args[1]) &&
            ((is_eligible(u, d, GT , s) && have_equal_increase(u, c, p, u, d, GT )) ||
             (is_eligible(u, d, GEQ, s) && have_equal_increase(u, c, p, u, d, GEQ)))) {
            site_info[u][c].retain[p] = false;
            break;
        }
    }
}

void checkL(int u, int c, int p, int s)
{
    for (int d = 0; d < units[u].num_sites; d++) {
        if (c != d &&
            sites[s + c].line == sites[s + d].line &&
            strcmp(sites[s + c].args[0], sites[s + d].args[0]) == 0 &&
            atoi(sites[s + c].args[1]) > atoi(sites[s + d].args[1]) &&
            ((is_eligible(u, d, LT , s) && have_equal_increase(u, c, p, u, d, LT )) ||
             (is_eligible(u, d, LEQ, s) && have_equal_increase(u, c, p, u, d, LEQ)))) {
            site_info[u][c].retain[p] = false;
            break;
        }
    }
}

void cull_preds_aggressively2()
{
    int u, c, s;

    for (s = 0, u = 0; u < num_units; s += units[u].num_sites, u++) {
        if (units[u].scheme_code == 'S') {
	    for (c = 0; c < units[u].num_sites; c++) {
                if (is_eligible(u, c, GT , s))
                    checkG(u, c, GT , s);
                if (is_eligible(u, c, GEQ, s))
                    checkG(u, c, GEQ, s);
                if (is_eligible(u, c, LT , s))
                    checkL(u, c, LT , s);
                if (is_eligible(u, c, LEQ, s))
                    checkL(u, c, LEQ, s);
            }
        }
    }
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
	if (!strcmp(argv[i], "-prefix")) {
	    i++;
	    prefix = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-r")) {
	    i++;
	    result_summary_xml_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-h")) {
	    puts("Usage: compute-results <options>\n"
                 "    -e       <experiment-name>\n"
                 "    -d       <program-src-dir>\n"
                 "    -c       <confidence>\n"
                 "    -prefix  <views-prefix>\n"
                 "(r) -s       <sruns-txt-file>\n"
                 "(r) -f       <fruns-txt-file>\n"
                 "(r) -cr      <compact-report-path-fmt>\n"
                 "(w) -p       <preds-txt-file>\n"
                 "(w) -r       <result-summary-xml-file>\n");
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
	!prefix ||
        !result_summary_xml_file) {
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

    site_info.resize(num_units);
    for (int u = 0; u < num_units; u++) site_info[u].resize(units[u].num_sites);

    for (cur_run = 0; cur_run < num_runs; cur_run++) {
        if (!is_srun[cur_run] && !is_frun[cur_run])
            continue;

        printf("r %d\n", cur_run);
        char s[1000];
        sprintf(s, compact_report_path_fmt, cur_run);

        FILE* fp = fopen(s, "r");
        assert(fp);

        process_report(fp, process_s_site, process_s_site, process_b_site, process_g_site);

        fclose(fp);
    }

    cull_preds();
    cull_preds_aggressively1();
    cull_preds_aggressively2();

    print_retained_preds();
    print_result_summary();

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
