// Usage: compute -p <program_name> -r <report_path_fmt> -d <program_src_dir> -c <confidence>
// Creates files PRED_TXT_FILE and PRED_HDR_FILE and RESULT_SUMMARY_FILE

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "units.h"
#include "classify_runs.h"

struct site_summary_t {
    int S[6], F[6];
    int os, of;
    site_summary_t() 
    { 
        os = of = 0;
        for (int i = 0; i < 6; i++) {
            S[i] = 0;
            F[i] = 0;
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
char* report_path_fmt = NULL;
char* program_src_dir = NULL;

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

void process_cmdline(int, char**);
void print_result_summary();

int main(int argc, char** argv)
{
    int i, j, k, x, y, z, w, u, c, p;

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

    for (i = 0; i < num_runs; i++) {
        if (!is_srun[i] && !is_frun[i])
            continue;
        printf("r %d\n", i);
        char file[1000];
        sprintf(file, report_path_fmt, i);
        FILE* fp = fopen(file, "r");
        assert(fp);

        while (1) {
            fscanf(fp, "%d\n", &j);
            if (feof(fp))
                break;
            switch (units[j].s[0]) {
            case 'S':
            case 'R':
                for (k = 0; k < units[j].c; k++) {
                    fscanf(fp, "%d %d %d\n", &x, &y, &z); 
                    if (x + y + z > 0) {
                        obs(i, j, k);
                        if (x > 0) {
                            inc(i, j, k, 0);
                            inc(i, j, k, 3);
                            inc(i, j, k, 5);
                            if (y > 0) {
                                inc(i, j, k, 1);
                                inc(i, j, k, 2);
                                if (z > 0)
                                    inc(i, j, k, 4);
                            } else if (z > 0) {
                                inc(i, j, k, 1);
                                inc(i, j, k, 4);
                            }
                        } else {
                            inc(i, j, k, 1);
                            if (y > 0) {
                                inc(i, j, k, 2);
                                inc(i, j, k, 5);
                                if (z > 0) {
                                    inc(i, j, k, 3);
                                    inc(i, j, k, 4);
                                }
                            } else {
                                inc(i, j, k, 3);
                                inc(i, j, k, 4);
                            }
                        }
                    }
                }
                break;
            case 'B':
                for (k = 0; k < units[j].c; k++) {
                    fscanf(fp, "%d %d\n", &x, &y); 
                    if (x + y > 0) {
                        obs(i, j, k);
                        if (x > 0) inc(i, j, k, 0);
                        if (y > 0) inc(i, j, k, 1);
                    }
                }
                break;
            case 'U':
                for (k = 0; k < units[j].c; k++) {
                    fscanf(fp, "%d %d %d %d\n", &x, &y, &z, &w);
                    if (x + y + z + w > 0) {
                        obs(i, j, k);
                        if (x > 0) inc(i, j, k, 0);
                        if (y > 0) inc(i, j, k, 1);
                        if (z > 0) inc(i, j, k, 2);
                        if (w > 0) inc(i, j, k, 3);
                    }
                }
                break;
            default:
                assert(0);
            }
        }
        fclose(fp);
    }

    for (u = 0; u < NUM_UNITS; u++) {
        for (c = 0; c < units[u].c; c++) {
            char site_name[10000];
            fscanf(sites_txt_fp, "%[^\n]s", site_name);
            fgetc (sites_txt_fp);
            print_site_summary(u, c, site_name);
        }
    }

    print_result_summary();

    for (i = 0; i < NUM_UNITS; i++)
        delete [] (site_summary[i]);

    fprintf(preds_hdr_fp, "};\n\n#endif\n");

    fclose(sites_txt_fp);
    fclose(preds_txt_fp);
    fclose(preds_hdr_fp);
    fclose(result_summary_fp);
    return 0;
}

void print_result_summary()
{
    time_t t;
    time(&t);

    fprintf(result_summary_fp, "<html><body>\n<table>\n<tr>\n"
                               "<td align=middle><font size=\"+2\">Cooperative Bug Isolation Report</font></td>\n"
                               "<td rowSpan=2 align=right>"
                               "<a href=\"http://www.cs.berkeley.edu/~liblit/sampler/\"><img src=\"http://www.cs.berkeley.edu/~liblit/sampler/logo.png\" style=\"border-style: none\"></a></td>\n</tr>\n"
                               "</table>\n<br>\n");
    fprintf(result_summary_fp, "Experiment name: %s\n<p>\n", program_name);
    fprintf(result_summary_fp, "Generated on %s<p>\n", ctime(&t));
    fprintf(result_summary_fp, "# runs: %d [successful: %d failing: %d discarded: %d]\n<p>\n", num_runs, num_sruns, num_fruns, num_runs - (num_sruns + num_fruns));
    fprintf(result_summary_fp, "# predicates instrumented: %d [branch: %d return: %d scalar: %d]\n<p>\n",
        NUM_B_PREDS + NUM_R_PREDS + NUM_S_PREDS, NUM_B_PREDS, NUM_R_PREDS, NUM_S_PREDS);
    fprintf(result_summary_fp, "# predicates retained: %d [branch: %d return: %d scalar: %d]\n<p>\n",
        num_b_preds + num_r_preds + num_s_preds, num_b_preds, num_r_preds, num_s_preds);
    fprintf(result_summary_fp, "Confidence: %d%%\n<p>\n", conf_percent);

    
    fprintf(result_summary_fp, "<table border=1>\n<tr>\n"
                               "<td></td>\n"
                               "<td align=middle>lower bound of<br>confidence interval</td>\n"
                               "<td align=middle>increase<br>score</td>\n"
                               "<td align=middle>fail<br>score</td>\n"
                               "<td align=middle>true in<br># F runs</td>\n"
                               "</tr>\n<tr>\n"
                               "<td>branch</td>\n"
                               "<td align=middle><a href=\"B_lb.html\">X</a></td>\n"
                               "<td align=middle><a href=\"B_is.html\">X</a></td>\n"
                               "<td align=middle><a href=\"B_fs.html\">X</a></td>\n"
                               "<td align=middle><a href=\"B_nf.html\">X</a></td>\n"
                               "</tr>\n<tr>\n"
                               "<td>return</td>\n"
                               "<td align=middle><a href=\"R_lb.html\">X</a></td>\n"
                               "<td align=middle><a href=\"R_is.html\">X</a></td>\n"
                               "<td align=middle><a href=\"R_fs.html\">X</a></td>\n"
                               "<td align=middle><a href=\"R_nf.html\">X</a></td>\n"
                               "</tr>\n<tr>\n"
                               "<td>scalar</td>\n"
                               "<td align=middle><a href=\"S_lb.html\">X</a></td>\n"
                               "<td align=middle><a href=\"S_is.html\">X</a></td>\n"
                               "<td align=middle><a href=\"S_fs.html\">X</a></td>\n"
                               "<td align=middle><a href=\"S_nf.html\">X</a></td>\n"
                               "</tr>\n<tr>\n"
                               "<td>all</td>\n"
                               "<td align=middle><a href=\"all_lb.html\">X</a></td>\n"
                               "<td align=middle><a href=\"all_is.html\">X</a></td>\n"
                               "<td align=middle><a href=\"all_fs.html\">X</a></td>\n"
                               "<td align=middle><a href=\"all_nf.html\">X</a></td>\n"
                               "</tr>\n</table>\n</body></html>\n");
}

void process_cmdline(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-p")) {
            i++;
            program_name = argv[i];
            continue;
        }
        if (!strcmp(argv[i], "-r")) {
            i++;
            report_path_fmt = argv[i];
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
            printf("Usage: compute -p program_name -r report_path_format -d program_src_dir -c confidence\n");
            exit(0);
        }
        printf("Illegal option: %s\n", argv[i]);
        exit(1);
    }

    if (!program_name || !report_path_fmt || !program_src_dir) {
        printf("Incorrect usage; try -h\n");
        exit(1);
    }
}

