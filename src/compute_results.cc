#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include "classify_runs.h"
#include "scaffold.h"

#define CBI_WEBPAGE "http://www.cs.berkeley.edu/~liblit/sampler/"

struct site_summary_t {
    int S[6], F[6];
    int os, of;
    bool is_pivot[6];
    bool is_true_pivot[6];
    site_summary_t()
    {
	os = of = 0;
	for (int i = 0; i < 6; i++) {
	    S[i] = 0;
	    F[i] = 0;
	    is_pivot[i] = false;
	}
    }
};

site_summary_t* site_summary[NUM_UNITS];

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
char* sruns_file = NULL;
char* fruns_file = NULL;
int   confidence = -1;
char* compact_sites_file = NULL;
char* compact_report_path_fmt = NULL;
char* trace_file = NULL;
char* preds_full_file = NULL;
char* preds_abbr_file = NULL;
char* preds_hdr_file = NULL;
char* result_summary_file = NULL;

float conf;

int num_s_preds = 0;
int num_r_preds = 0;
int num_b_preds = 0;

FILE* sites_fp = NULL;
FILE* trace_fp = NULL;
FILE* preds_full_fp = NULL;
FILE* preds_abbr_fp = NULL;
FILE* preds_hdr_fp = NULL;
FILE* result_summary_fp = NULL;

#define INC(r, u, c, p)            \
{                                  \
    if (is_srun[r])                \
	site_summary[u][c].S[p]++; \
    else if (is_frun[r])           \
	site_summary[u][c].F[p]++; \
}

#define OBS(r, u, c)              \
{                                 \
    if (is_srun[r])               \
	site_summary[u][c].os++;  \
    else if (is_frun[r])          \
	site_summary[u][c].of++;  \
}


struct pred_name {
    char* file;
    char* line;
    char* func;
    char* pred;
};

void extract_pred_name(char* s, pred_name* n)
{
    n->file = s;
    n->line = strchr(n->file, '\t'); *(n->line) = '\0'; n->line++;
    n->func = strchr(n->line, '\t'); *(n->func) = '\0'; n->func++;
    n->pred = strchr(n->func, '\t'); *(n->pred) = '\0'; n->pred++;
}

void restore_site_name(pred_name* n)
{
    fprintf(preds_full_fp, "</td><td>%s</td><td><a href=\"%s/%s#%s\">%s:%s</a>\n",
	n->func, program_src_dir, n->file, n->line, n->file, n->line);
    fprintf(preds_hdr_fp , "</td><td>%s</td><td><a href=\\\"%s/%s#%s\\\">%s:%s</a>\",\n",
	n->func, program_src_dir, n->file, n->line, n->file, n->line);

    *(n->line - 1) = '\t';
    *(n->func - 1) = '\t';
    *(n->pred - 1) = '\t';
}

char  scalar_op[3] = { '<', '=', '>' };
char* return_op[3] = { "< 0", "= 0", "> 0" };

bool is_bug_predictor(int u, int c, int p)
{
    int s  = site_summary[u][c].S[p];
    int f  = site_summary[u][c].F[p];
    int os = site_summary[u][c].os;
    int of = site_summary[u][c].of;

    float fs = ((float)  f) / ( s +  f);
    float co = ((float) of) / (os + of);
    float in = fs - co;
    float lb = in - conf * sqrt(((fs * (1 - fs)) / (f + s)) + ((co * (1 - co))/(of + os)));

    if (lb > 0 && s + f >= 10) return true;
    return false;
}

void print_pred_stats(int u, int c, int p)
{
    int s  = site_summary[u][c].S[p];
    int f  = site_summary[u][c].F[p];
    int os = site_summary[u][c].os;
    int of = site_summary[u][c].of;

    float fs = ((float)  f) / ( s +  f);
    float co = ((float) of) / (os + of);
    float in = fs - co;
    float lb = in - conf * sqrt(((fs * (1 - fs)) / (f + s)) + ((co * (1 - co))/(of + os)));

    fprintf(preds_full_fp, "%c %d %d %d %.2f %.2f %.2f %.2f %d %d %d %d ",
	units[u].s[0], u, c, p, lb, in, fs, co, s, f, os, of);
    fprintf(preds_abbr_fp, "%d %d %d\n", u, c, p);
    fprintf(preds_hdr_fp , "\t\"%c %d %d %d %.2f %.2f %.2f %.2f %d %d %d %d ",
	units[u].s[0], u, c, p, lb, in, fs, co, s, f, os, of);
}

void print_branch_pred(FILE* fp, char* pred, char* op)
{
    fprintf(fp, "%s %s", pred, op);
}

void print_return_pred(FILE* fp, char* pred, char* op, bool is_neg)
{
    if (is_neg) fputs("!(", fp);
    fprintf(fp, "%s %s", pred, op);
    if (is_neg) fputc(')',  fp);
}

void print_scalar_pred(FILE* fp, char* pred, char op, bool is_neg)
{
    if (is_neg) fputs("!(", fp);
    char* s = strchr(pred, '$');
    *s = op;
    fputs(pred, fp);
    *s = '$';
    if (is_neg) fputc(')', fp);
}

void print_site_summary(int u, int c, char* site_name)
{
    pred_name n;
    int p, count = 0;

#define LT  0
#define GEQ 1
#define EQ  2
#define NEQ 3
#define GT  4
#define LEQ 5

    bool a[6] = { false, // 0 LT
		  false, // 1 GEQ
		  false, // 2 EQ
		  false, // 3 NEQ
		  false, // 4 GT
		  false  // 5 LEQ
		};

    switch (units[u].s[0]) {
    case 'S':
	for (p = 0; p < 6; p++)
	    if (is_bug_predictor(u, c, p)) {
		count++;
		a[p] = true;
	    }

	switch (count) {
	case 2:
	    if (a[LT]) {
		if (a[LEQ])
		    a[LEQ] = false;
		else if (a[NEQ])
		    a[NEQ] = false;
	    } else if (a[GT]) {
		if (a[GEQ])
		    a[GEQ] = false;
		else if (a[NEQ])
		    a[NEQ] = false;
	    } else if (a[EQ]) {
		if (a[LEQ])
		    a[LEQ] = false;
		else if (a[GEQ])
		    a[GEQ] = false;
	    }
	    break;
	case 3:
	    if (a[LT] && a[EQ] && a[LEQ])
		a[LT] = a[EQ] = false;
	    else if (a[GT] && a[EQ] && a[GEQ])
		a[GT] = a[EQ] = false;
	    else if (a[LT] && a[GT] && a[NEQ])
		a[LT] = a[GT] = false;
	    else if (a[LT] && a[LEQ] && a[NEQ])
		a[LEQ] = a[NEQ] = false;
	    else if (a[GT] && a[GEQ] && a[NEQ])
		a[GEQ] = a[NEQ] = false;
	    else if (a[EQ] && a[LEQ] && a[GEQ])
		a[LEQ] = a[GEQ] = false;
	    break;
	}

	for (p = 0; p < 6; p++)
	    if (a[p]) {
		num_s_preds++;
		print_pred_stats(u, c, p);
		extract_pred_name(site_name, &n);
		print_scalar_pred(preds_full_fp, n.pred, scalar_op[p / 2], p % 2 ? true : false);
		print_scalar_pred(preds_hdr_fp , n.pred, scalar_op[p / 2], p % 2 ? true : false);
		restore_site_name(&n);
	    }
	break;
    case 'R':
	for (p = 0; p < 6; p++)
	    if (is_bug_predictor(u, c, p)) {
		count++;
		a[p] = true;
	    }

	switch (count) {
	case 2:
	    if (a[LT]) {
		if (a[LEQ])
		    a[LEQ] = false;
		else if (a[NEQ])
		    a[NEQ] = false;
	    } else if (a[GT]) {
		if (a[GEQ])
		    a[GEQ] = false;
		else if (a[NEQ])
		    a[NEQ] = false;
	    } else if (a[EQ]) {
		if (a[LEQ])
		    a[LEQ] = false;
		else if (a[GEQ])
		    a[GEQ] = false;
	    }
	    break;
	case 3:
	    if (a[LT] && a[EQ] && a[LEQ])
		a[LT] = a[EQ] = false;
	    else if (a[GT] && a[EQ] && a[GEQ])
		a[GT] = a[EQ] = false;
	    else if (a[LT] && a[GT] && a[NEQ])
		a[LT] = a[GT] = false;
	    else if (a[LT] && a[LEQ] && a[NEQ])
		a[LEQ] = a[NEQ] = false;
	    else if (a[GT] && a[GEQ] && a[NEQ])
		a[GEQ] = a[NEQ] = false;
	    else if (a[EQ] && a[LEQ] && a[GEQ])
		a[LEQ] = a[GEQ] = false;
	    break;
	}

	for (p = 0; p < 6; p++)
	    if (a[p]) {
		num_r_preds++;
		print_pred_stats(u, c, p);
		extract_pred_name(site_name, &n);
		print_return_pred(preds_full_fp, n.pred, return_op[p / 2], p % 2 ? true : false);
		print_return_pred(preds_hdr_fp , n.pred, return_op[p / 2], p % 2 ? true : false);
		restore_site_name(&n);
	    }
	break;
    case 'B':
	if (is_bug_predictor(u, c, 0)) {
	    num_b_preds++;
	    print_pred_stats(u, c, 0);
	    extract_pred_name(site_name, &n);
	    print_branch_pred(preds_full_fp, n.pred, "is FALSE");
	    print_branch_pred(preds_hdr_fp , n.pred, "is FALSE");
	    restore_site_name(&n);
	}
	if (is_bug_predictor(u, c, 1)) {
	    num_b_preds++;
	    print_pred_stats(u, c, 1);
	    extract_pred_name(site_name, &n);
	    print_branch_pred(preds_full_fp, n.pred, "is TRUE");
	    print_branch_pred(preds_hdr_fp , n.pred, "is TRUE");
	    restore_site_name(&n);
	}
	break;
    default: assert(0);
    }
}

void process_site(FILE* fp, int r, int u, int c)
{
    int x, y, z;

    switch (units[u].s[0]) {
    case 'S':
    case 'R':
	fscanf(fp, "%d %d %d\n", &x, &y, &z);
	if (x + y + z > 0) {
	    OBS(r, u, c)
	    if (x > 0)
		INC(r, u, c, 0)
	    if (y + z > 0)
		INC(r, u, c, 1)
	    if (y > 0)
		INC(r, u, c, 2)
	    if (x + z > 0)
		INC(r, u, c, 3)
	    if (z > 0)
		INC(r, u, c, 4)
	    if (x + y > 0)
		INC(r, u, c, 5)
	}
	break;
    case 'B':
	fscanf(fp, "%d %d\n", &x, &y);
	if (x + y > 0) {
	    OBS(r, u, c)
	    if (x > 0) INC(r, u, c, 0)
	    if (y > 0) INC(r, u, c, 1)
	}
	break;
    default:
	assert(0);
    }
}

void discard_run(int r)
{
    if (is_srun[r]) {
	is_srun[r] = false;
	num_sruns--;
	return;
    }
    if (is_frun[r]) {
	is_frun[r] = false;
	num_fruns--;
	return;
    }
}

inline bool XOR(bool a, bool b) { return (!a && b) || (a && !b); }

void process_run(FILE* fp, int r, int u, int c)
{
    int x, y, z;

    switch (units[u].s[0]) {
    case 'S':
    case 'R':
	fscanf(fp, "%d %d %d\n", &x, &y, &z);
	if (site_summary[u][c].is_pivot[0] && XOR(site_summary[u][c].is_true_pivot[0], x))
	    discard_run(r);
	if (site_summary[u][c].is_pivot[1] && XOR(site_summary[u][c].is_true_pivot[1], y + z))
	    discard_run(r);
	if (site_summary[u][c].is_pivot[2] && XOR(site_summary[u][c].is_true_pivot[2], y))
	    discard_run(r);
	if (site_summary[u][c].is_pivot[3] && XOR(site_summary[u][c].is_true_pivot[3], x + z))
	    discard_run(r);
	if (site_summary[u][c].is_pivot[4] && XOR(site_summary[u][c].is_true_pivot[4], z))
	    discard_run(r);
	if (site_summary[u][c].is_pivot[5] && XOR(site_summary[u][c].is_true_pivot[5], x + y))
	    discard_run(r);
	break;
    case 'B':
	fscanf(fp, "%d %d\n", &x, &y);
	if (site_summary[u][c].is_pivot[0] && XOR(site_summary[u][c].is_true_pivot[0], x))
	    discard_run(r);
	if (site_summary[u][c].is_pivot[1] && XOR(site_summary[u][c].is_true_pivot[1], y))
	    discard_run(r);
	break;
    default:
	assert(0);
    }
}

void print_result_summary()
{
    time_t t;
    time(&t);

    fputs("<table>\n<tr>\n"
	  "<td align=middle><font size=\"+2\">Cooperative Bug Isolation Report</font></td>\n"
	  "<td rowSpan=2 align=right>"
	  "<a href=\"" CBI_WEBPAGE "\"><img src=\"http://www.cs.berkeley.edu/~liblit/sampler/logo.png\" style=\"border-style: none\"></a></td>\n</tr>\n"
	  "</table>\n<br>\n",
	  result_summary_fp);
    fprintf(result_summary_fp, "Experiment name: %s\n<p>\n", experiment_name);
    fprintf(result_summary_fp, "Generated on %s<p>\n", ctime(&t));
    fprintf(result_summary_fp, "# runs: %d [successful: %d failing: %d discarded: %d]\n<p>\n",
	num_runs, num_sruns, num_fruns, num_runs - (num_sruns + num_fruns));
    fprintf(result_summary_fp, "# predicates instrumented: %d [branch: %d return: %d scalar: %d]\n<p>\n",
	NUM_B_PREDS + NUM_R_PREDS + NUM_S_PREDS, NUM_B_PREDS, NUM_R_PREDS, NUM_S_PREDS);
    fprintf(result_summary_fp, "# predicates retained: %d [branch: %d return: %d scalar: %d]\n<p>\n",
	num_b_preds + num_r_preds + num_s_preds, num_b_preds, num_r_preds, num_s_preds);
    fprintf(result_summary_fp, "Confidence: %d%%\n<p>\n", confidence);
}

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
	if (!strcmp(argv[i], "-s")) {
	    i++;
	    sruns_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-f")) {
	    i++;
	    fruns_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-c")) {
	    i++;
	    confidence = atoi(argv[i]);
	    assert(confidence >= 90 && confidence < 100);
	    continue;
	}
	if (!strcmp(argv[i], "-cs")) {
	    i++;
	    compact_sites_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-cr")) {
	    i++;
	    compact_report_path_fmt = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-t")) {
	    i++;
	    trace_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-pf")) {
	    i++;
	    preds_full_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-pa")) {
	    i++;
	    preds_abbr_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-ph")) {
	    i++;
	    preds_hdr_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-r")) {
	    i++;
	    result_summary_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-h")) {
	    puts("Usage: compute-results -e <experiment-name> -d <program-src-dir> -s <sruns-file> -f <fruns-file> -c <confidence> -cs <compact-sites-file> -cr <compact-report-path-fmt> -t <trace-file> -pf <preds-full-file> -pa <preds-abbr-file> -ph <preds-hdr-file> -r <result-summary-file>");
	    exit(0);
	}
	printf("Illegal option: %s\n", argv[i]);
	exit(1);
    }

    if (!experiment_name || !program_src_dir || !sruns_file || !fruns_file || confidence == -1 ||
	!compact_sites_file || !compact_report_path_fmt ||
	!preds_full_file || !preds_abbr_file || !preds_hdr_file || !result_summary_file) {
	puts("Incorrect usage; try -h");
	exit(1);
    }
}

void process_trace_file()
{
    int u, c, p, b;

    if (!trace_file)
	return;

    trace_fp = fopen(trace_file, "r"); assert(trace_fp);

    while (1) {
	fscanf(trace_fp, "%d %d %d %d", &u, &c, &p, &b);
	if (feof(trace_fp))
	    break;
	assert(u >= 0 && u < NUM_UNITS );
	assert(c >= 0 && c < units[u].c);
	assert(p >= 0 && p < 6);
	assert(b == 0 || b == 1);
	site_summary[u][c].is_pivot[p] = true;
	site_summary[u][c].is_true_pivot[p] = b;
    }

    fclose(trace_fp);
    scaffold(compact_report_path_fmt, process_run);
}

int main(int argc, char** argv)
{
    int u, c, p;

    process_cmdline(argc, argv);

    classify_runs(sruns_file, fruns_file);

    conf = conf_map[confidence - 90];

    sites_fp = fopen(compact_sites_file, "r");
    assert(sites_fp);
    preds_full_fp = fopen(preds_full_file, "w");
    assert(preds_full_fp);
    preds_abbr_fp = fopen(preds_abbr_file, "w");
    assert(preds_abbr_fp);
    preds_hdr_fp = fopen(preds_hdr_file, "w");
    assert(preds_hdr_fp);
    result_summary_fp = fopen(result_summary_file, "w");
    assert(result_summary_fp);

    fputs("#ifndef PREDS_H\n#define PREDS_H\n\n", preds_hdr_fp);
    fputs("const char* preds[] = {\n", preds_hdr_fp);

    for (u = 0; u < NUM_UNITS; u++) {
	site_summary[u] = new (site_summary_t) [units[u].c];
	assert(site_summary[u]);
    }

    process_trace_file();

    scaffold(compact_report_path_fmt, process_site);

    for (u = 0; u < NUM_UNITS; u++) {
	for (c = 0; c < units[u].c; c++) {
	    char site_name[10000];
	    fscanf(sites_fp, "%[^\n]s", site_name); fgetc(sites_fp);
	    print_site_summary(u, c, site_name);
	}
    }

    print_result_summary();

    for (u = 0; u < NUM_UNITS; u++)
	delete [] (site_summary[u]);

    fputs("};\n\n#endif\n", preds_hdr_fp);

    fclose(sites_fp);
    fclose(preds_full_fp);
    fclose(preds_abbr_fp);
    fclose(preds_hdr_fp);
    fclose(result_summary_fp);
    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
