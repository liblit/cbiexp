#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iostream>
#include <vector>
#include "classify_runs.h"
#include "units.h"
#include "sites.h"
#include "utils.h"

const int NUM_FUNS = 31;

const char* funs[NUM_FUNS] = {
    "region_main",
        "handle_options",
            "open_file",
            "read_database",
        "base_calc",
        "read_manifest",
            "string2lang",
    "process_file_pass1",
        "initialize_token_sequence",
        "read_token",
                "yylex",
                    "code",
        "first_hash",
        "store_passage",
        "process",
            "lookup_tile",
            "lookup_tile_in_file",
            "which_duplicate_in_file",
            "next_file_for_tile",
                 "num_copies_of_tile_in_file",
            "new_match_info",
    "write_database",
        "write_string_to_file",
    "passage_leq",
    "dont_match_common_tiles",
        "num_copies_of_tile",
    "build_passage_array_index",
    "process_file_pass2",
        "new_resultinfo",
        "match_info_leq",
        "size_of_match"
};

int fun_indx(char* f)
{
    for (int i = 0; i < NUM_FUNS; i++) {
        if (!strcmp(funs[i], f))
            return i;
    }
    assert(0);
}

bool precedes(int s, int t)
{
   // does s precede t?

   if (!strcmp(sites[s].fun, sites[t].fun))
       return (sites[s].line < sites[t].line) ? true : false;
   else
       return (fun_indx(sites[s].fun) < fun_indx(sites[t].fun)) ? true : false;
}

using namespace std;

struct pred_ctrs_t {
    int s;
    int f;
    int os;
    int of;
    pred_ctrs_t() {
        s = f = os = of = 0;
    }
};

struct pred_info_t {
    int u, c, p, s;
    vector<pred_ctrs_t> pred_ctrs;
    pred_info_t() { }
};

vector<pred_info_t> pred_info;

inline int U(int i) { return pred_info[i].u; }
inline int C(int i) { return pred_info[i].c; }
inline int P(int i) { return pred_info[i].p; }
inline int S(int i) { return pred_info[i].s; }

struct site_info_t {
    bool exists;
    // the following fields are defined iff (exists == true)
    bool obs, tru[6];
    site_info_t() { exists = false; }
};

vector<vector<site_info_t> > site_info;

int confidence = -1;
int num_preds  = -1;
char* preds_txt_file = NULL;
char* sruns_txt_file = NULL;
char* fruns_txt_file = NULL;
char* compact_report_path_fmt = NULL;

void process_s_site(int u, int c, int x, int y, int z)
{
    if (!site_info[u][c].exists)
        return;
    site_info[u][c].obs    = (x + y + z > 0) ? true : false;
    site_info[u][c].tru[0] = (x > 0) ? true : false;
    site_info[u][c].tru[1] = (y + z > 0) ? true : false;
    site_info[u][c].tru[2] = (y > 0) ? true : false;
    site_info[u][c].tru[3] = (x + z > 0) ? true : false;
    site_info[u][c].tru[4] = (z > 0) ? true : false;
    site_info[u][c].tru[5] = (x + y > 0) ? true : false;
}

void process_b_site(int u, int c, int x, int y)
{
    if (!site_info[u][c].exists)
        return;
    site_info[u][c].obs    = (x + y > 0) ? true : false;
    site_info[u][c].tru[0] = (x > 0) ? true : false;
    site_info[u][c].tru[1] = (y > 0) ? true : false;
}

/****************************************************************************
 * Processing command line options
 ***************************************************************************/

void process_cmdline(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-c")) {
            i++;
            confidence = atoi(argv[i]);
            assert(confidence >= 90 && confidence < 100);
            continue;
        }
	if (!strcmp(argv[i], "-n")) {
	    i++;
	    num_preds = atoi(argv[i]);
	    continue;
	}
	if (!strcmp(argv[i], "-p")) {
	    i++;
	    preds_txt_file = argv[i];
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
	if (!strcmp(argv[i], "-h")) {
	    puts("Usage: choose_pivot <options>\n"
                 "    -c  <confidence>\n"
                 "    -n  <num-preds>\n"
                 "(r) -p  <preds-txt-file>\n"
                 "(r) -s  <sruns-txt-file>\n"
                 "(r) -f  <fruns-txt-file>\n"
                 "(r) -cr <compact-report-path-fmt>\n"
            );
	    exit(0);
	}
	printf("Illegal option: %s\n", argv[i]);
	exit(1);
    }

    if (confidence == -1 ||
        num_preds  == -1 ||
        !preds_txt_file ||
        !sruns_txt_file ||
        !fruns_txt_file ||
        !compact_report_path_fmt) {
	puts("Incorrect usage; try -h");
	exit(1);
    }
}

/****************************************************************************
 * MAIN
 ***************************************************************************/

int main(int argc, char** argv)
{
    int cur_run, u, c, p, s, i, j, x, y, z;
    FILE* fp;

    process_cmdline(argc, argv);

    classify_runs(sruns_txt_file, fruns_txt_file);

    pred_info.resize(num_preds);
    for (i = 0; i < num_preds; i++) pred_info[i].pred_ctrs.resize(num_preds);

    site_info.resize(num_units);
    for (i = 0; i < num_units; i++) site_info[i].resize(units[i].num_sites);

    fp = fopen(preds_txt_file, "r");
    assert(fp);

    for (i = 0; i < num_preds; i++) {
        fscanf(fp, "%d %d %d %d", &u, &c, &p, &s);
        pred_info[i].u = u;
        pred_info[i].c = c;
        pred_info[i].p = p;
        pred_info[i].s = s;
        site_info[u][c].exists = true;
    }

    fclose(fp);

    for (cur_run = 0; cur_run < num_runs; cur_run++) {
        if (!is_srun[cur_run] && !is_frun[cur_run])
            continue;

        printf("r %d\n", cur_run);
        char s[1000];
        sprintf(s, compact_report_path_fmt, cur_run);

        FILE* fp = fopen(s, "r");
        assert(fp);

        process_report(fp, process_s_site, process_s_site, process_b_site);

        fclose(fp);

        for (i = 0; i < num_preds; i++) {
            if (!site_info[U(i)][C(i)].tru[P(i)])
                continue;
            for (j = 0; j < num_preds; j++) {
                if (!site_info[U(j)][C(j)].obs)
                    continue;
                if (is_srun[cur_run])
                    pred_info[i].pred_ctrs[j].os++;
                else
                    pred_info[i].pred_ctrs[j].of++;
                if (!site_info[U(j)][C(j)].tru[P(j)])
                    continue;
                if (is_srun[cur_run])
                    pred_info[i].pred_ctrs[j].s++;
                else
                    pred_info[i].pred_ctrs[j].f++;
            }
        }
    }

    FILE* rfp = fopen("results.html", "w");
    assert(rfp);

    for (i = 0; i < num_preds; i++) {
        char file[100];
        sprintf(file, "/moa/sc1/root2/%d_%d_%d.html", U(i), C(i), P(i));
        fp = fopen(file, "w");
        assert(fp);

        fputs("<html>"
              "<head>"
              "<link type=\"text/css\" rel=\"stylesheet\" href=\"http://www.stanford.edu/~mhn/style.css\"/>"
              "</head>"
              "<body>"
              "<table>\n", fp);

        int all = 0, pre = 0;

        for (j = 0; j < num_preds; j++) {
            if (i == j)
                continue;

            int s  = pred_info[i].pred_ctrs[j].s ;
            int f  = pred_info[i].pred_ctrs[j].f ;
            int os = pred_info[i].pred_ctrs[j].os;
            int of = pred_info[i].pred_ctrs[j].of;
            pred_stat ps = compute_pred_stat(s, f, os, of, confidence);

            if (retain_pred(s, f, ps.lb)) {
                all++;
                fputs("<tr>\n", fp);
                if (precedes(S(j), S(i))) {
                    pre++;
                    fputs("<td>PRE</td>", fp);
                } else
                    fputs("<td>SUC</td>", fp);
                print_pred_full(fp, ps, s, f, S(j), P(j));
                fputs("</tr>\n", fp);
            }
        }

        fputs("</table>"
              "</body>"
              "</html>", fp);

        fclose(fp);

        fputs("<tr>\n", rfp);
        fprintf(rfp, "<td>ALL: %d PRE: %d</td><td><a href=\"%d_%d_%d.html\">list</a></td>\n", all, pre, U(i), C(i), P(i));
        print_pred_name(rfp, S(i), P(i));
        fputs("</tr>\n", rfp);
    }

    fclose(rfp);

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
