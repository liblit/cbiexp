// Usage: compute -p <program_name> -d <program_src_dir> [-c <confidence>]
// Creates files PRED_TXT_FILE and PRED_HDR_FILE and RESULT_SUMMARY_FILE

#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include "def.h"
#include UNITS_HDR_FILE
#include "classify_runs.h"
#include "scaffold.h"

struct site_summary_t {
    int S[6], F[6];
    int os, of;
    bool is_pivot[6];
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

int   conf_percent = 95;
float conf;

char* program_name = NULL;
char* program_src_dir = NULL;

int num_pivots = 0;

int num_s_preds = 0;
int num_r_preds = 0;
int num_b_preds = 0;

FILE* sites_txt_fp = NULL;
FILE* preds_txt_fp = NULL;
FILE* preds_hdr_fp = NULL;
FILE* result_summary_fp = NULL;

#define inc(r, u, c, p)            \
{                                  \
    if (is_srun[r])                \
        site_summary[u][c].S[p]++; \
    else if (is_frun[r])           \
        site_summary[u][c].F[p]++; \
}

#define obs(r, u, c)              \
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
    fprintf(preds_txt_fp, "</td><td>%s</td><td><a href=\"%s/%s#%s\">%s:%s</a>\n",
        n->func, program_src_dir, n->file, n->line, n->file, n->line);
    fprintf(preds_hdr_fp, "</td><td>%s</td><td><a href=\\\"%s/%s#%s\\\">%s:%s</a>\",\n",
        n->func, program_src_dir, n->file, n->line, n->file, n->line);

    *(n->line - 1) = '\t';
    *(n->func - 1) = '\t';
    *(n->pred - 1) = '\t';
}

char  scalar_op[3] = { '<', '=', '>' };
char* return_op[3] = { "< 0", "= 0", "> 0" };

bool print_pred_summary(int u, int c, int p)
{
    int s  = site_summary[u][c].S[p];
    int f  = site_summary[u][c].F[p];
    int os = site_summary[u][c].os;
    int of = site_summary[u][c].of;

    float fs = ((float)  f) / ( s +  f);
    float co = ((float) of) / (os + of);
    float in = fs - co;
    float lb = in - conf * sqrt(((fs * (1 - fs)) / (f + s)) + ((co * (1 - co))/(of + os)));
    if (lb > 0 && s + f >= 10) {
        fprintf(preds_txt_fp,     "%c %d %d %d %.2f %.2f %.2f %.2f %d %d %d %d ", units[u].s[0], u, c, p, lb, in, fs, co, s, f, os, of);
        fprintf(preds_hdr_fp, "\t\"%c %d %d %d %.2f %.2f %.2f %.2f %d %d %d %d ", units[u].s[0], u, c, p, lb, in, fs, co, s, f, os, of);
        return true;
    }
    return false;
}

void print_branch_pred(FILE* fp, char* pred, char* op)
{
    fprintf(fp, "%s %s", pred, op);
}

void print_return_pred(FILE* fp, char* pred, char* op, bool is_neg)
{
    if (is_neg) fprintf(fp, "!(");
    fprintf(fp, "%s %s", pred, op);
    if (is_neg) fprintf(fp,  ")");
}

void print_scalar_pred(FILE* fp, char* pred, char op, bool is_neg)
{
    if (is_neg) fprintf(fp, "!(");
    char* s = strchr(pred, '$');
    *s = op;
    fprintf(fp, "%s", pred);
    *s = '$';
    if (is_neg) fprintf(fp,  ")");
}

bool print_site_summary(int u, int c, char* site_name)
{
    pred_name n;
    int p;

    switch (units[u].s[0]) {
    case 'S':
        for (p = 0; p < 6; p++)
            if (print_pred_summary(u, c, p)) {
                num_s_preds++;
                extract_pred_name(site_name, &n);
                print_scalar_pred(preds_txt_fp, n.pred, scalar_op[p / 2], p % 2 ? true : false);
                print_scalar_pred(preds_hdr_fp, n.pred, scalar_op[p / 2], p % 2 ? true : false);
                restore_site_name(&n);
            }
        break;
    case 'R':
        for (p = 0; p < 6; p++) 
            if (print_pred_summary(u, c, p)) {
                num_r_preds++;
                extract_pred_name(site_name, &n);
                print_return_pred(preds_txt_fp, n.pred, return_op[p / 2], p % 2 ? true : false);
                print_return_pred(preds_hdr_fp, n.pred, return_op[p / 2], p % 2 ? true : false);
                restore_site_name(&n);
            }
        break;
    case 'B':
        if (print_pred_summary(u, c, 0)) {
            num_b_preds++;
            extract_pred_name(site_name, &n);
            print_branch_pred(preds_txt_fp, n.pred, "is FALSE");
            print_branch_pred(preds_hdr_fp, n.pred, "is FALSE");
            restore_site_name(&n);
        }
        if (print_pred_summary(u, c, 1)) {
            num_b_preds++;
            extract_pred_name(site_name, &n);
            print_branch_pred(preds_txt_fp, n.pred, "is TRUE");
            print_branch_pred(preds_hdr_fp, n.pred, "is TRUE");
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
    assert(0);
}

void process_run(FILE* fp, int r, int u, int c)
{
    int x, y, z;

    switch (units[u].s[0]) {
    case 'S':
    case 'R':
        fscanf(fp, "%d %d %d\n", &x, &y, &z); 
        if (site_summary[u][c].is_pivot[0] && x == 0)
            discard_run(r);
        if (site_summary[u][c].is_pivot[1] && y + z == 0)
            discard_run(r);
        if (site_summary[u][c].is_pivot[2] && y == 0)
            discard_run(r);
        if (site_summary[u][c].is_pivot[3] && x + z == 0)
            discard_run(r);
        if (site_summary[u][c].is_pivot[4] && z == 0)
            discard_run(r);
        if (site_summary[u][c].is_pivot[5] && x + y == 0)
            discard_run(r);
        break;
    case 'B':
        fscanf(fp, "%d %d\n", &x, &y); 
        if (site_summary[u][c].is_pivot[0] && x == 0)
            discard_run(r);
        if (site_summary[u][c].is_pivot[1] && y == 0)
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

    fprintf(result_summary_fp,
        "<html><body>\n<table>\n<tr>\n"
        "<td align=middle><font size=\"+2\">Cooperative Bug Isolation Report</font></td>\n"
        "<td rowSpan=2 align=right>"
        "<a href=\"%s\"><img src=\"http://www.cs.berkeley.edu/~liblit/sampler/logo.png\" style=\"border-style: none\"></a></td>\n</tr>\n"
        "</table>\n<br>\n", CBI_WEBPAGE);
    fprintf(result_summary_fp, "Experiment name: %s\n<p>\n", program_name);
    fprintf(result_summary_fp, "Generated on %s<p>\n", ctime(&t));
    fprintf(result_summary_fp, "# runs: %d [successful: %d failing: %d discarded: %d]\n<p>\n", 
        num_runs, num_sruns, num_fruns, num_runs - (num_sruns + num_fruns));
    fprintf(result_summary_fp, "# predicates instrumented: %d [branch: %d return: %d scalar: %d]\n<p>\n",
        NUM_B_PREDS + NUM_R_PREDS + NUM_S_PREDS, NUM_B_PREDS, NUM_R_PREDS, NUM_S_PREDS);
    fprintf(result_summary_fp, "# predicates retained: %d [branch: %d return: %d scalar: %d]\n<p>\n",
        num_b_preds + num_r_preds + num_s_preds, num_b_preds, num_r_preds, num_s_preds);
    fprintf(result_summary_fp, "Confidence: %d%%\n<p>\n", conf_percent);
}

void process_cmdline(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-p")) {
            i++;
            program_name = argv[i];
            continue;
        }
        if (!strcmp(argv[i], "-d")) {
            i++;
            program_src_dir = argv[i];
            continue;
        }
        if (!strcmp(argv[i], "-c")) {
            i++;
            conf_percent = atoi(argv[i]);
            assert(conf_percent >= 90 && conf_percent < 100);
            continue;
        } 
        if (!strcmp(argv[i], "-h")) {
            printf("Usage: compute -p program_name -d program_src_dir [-c confidence]\n");
            exit(0);
        }
        printf("Illegal option: %s\n", argv[i]);
        exit(1);
    }

    if (!program_name || !program_src_dir) {
        printf("Incorrect usage; try -h\n");
        exit(1);
    }
}

int main(int argc, char** argv)
{
    int u, c, p;

    process_cmdline(argc, argv);

    conf = conf_map[conf_percent - 90];

    classify_runs();

    sites_txt_fp = fopen(SITES_TXT_FILE, "r"); assert(sites_txt_fp);
    preds_txt_fp = fopen(PREDS_TXT_FILE, "w"); assert(preds_txt_fp);
    preds_hdr_fp = fopen(PREDS_HDR_FILE, "w"); assert(preds_hdr_fp);
    result_summary_fp = fopen(RESULT_SUMMARY_FILE, "w"); assert(result_summary_fp);

    fprintf(preds_hdr_fp, "#ifndef PREDS_H\n#define PREDS_H\n\n");
    fprintf(preds_hdr_fp, "const char* preds[] = {\n");

    for (u = 0; u < NUM_UNITS; u++) {
        site_summary[u] = new (site_summary_t) [units[u].c];
        assert(site_summary[u]);
    }

    while (1) {
        fscanf(stdin, "%d %d %d", &u, &c, &p);
        if (feof(stdin))
            break;
        assert(u >= 0 && u < NUM_UNITS );
        assert(c >= 0 && c < units[u].c);
        assert(p >= 0 && p < 6);
        site_summary[u][c].is_pivot[p] = true;
        num_pivots++;
    }

    printf("%d pivots\n", num_pivots);

    if (num_pivots) scaffold(process_run);

    scaffold(process_site);

    for (u = 0; u < NUM_UNITS; u++) {
        for (c = 0; c < units[u].c; c++) {
            char site_name[10000];
            fscanf(sites_txt_fp, "%[^\n]s", site_name);
            fgetc (sites_txt_fp);
            print_site_summary(u, c, site_name);
        }
    }

    print_result_summary();

    for (u = 0; u < NUM_UNITS; u++)
        delete [] (site_summary[u]);

    fprintf(preds_hdr_fp, "};\n\n#endif\n");

    fclose(sites_txt_fp);
    fclose(preds_txt_fp);
    fclose(preds_hdr_fp);
    fclose(result_summary_fp);
    return 0;
}

