#include <cstdio>
#include <cmath>
#include <cassert>
#include "units.h"
#include "sites.h"
#include "utils.h"

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

const char* s_op[6] = { "<", ">=", "==", "!=", ">", "<=" };
const char* r_op[6] = { "<", ">=", "==", "!=", ">", "<=" };
const char* b_op[2] = { "is FALSE", "is TRUE" };

void print_pred_full(FILE* fp, pred_stat ps, int s, int f, int site, int p)
{
    print_pred_stat(fp, ps, s, f);
    print_pred_name(fp, site, p);
}

void print_pred_stat(FILE* fp, pred_stat ps, int s, int f)
{
    float f1 = ps.co * 100;
    float f2 = ps.lb * 100;
    float f3 = (ps.fs - (ps.co + ps.lb)) * 100;
    float f4 = (1.0 - ps.fs) * 100;
    fprintf(fp, "<td>"
                "<table class=\"scores\" style=\"width: %dpx\" "
                "title=\"Context: %.0f%% Lower Bound: %.0f%%, Increase: %.0f%%, Fail: %.0f%%, true in %d S and %d F runs\">"
                "<tr>"
                "<td class=\"f1\" style=\"width: %.2f%%\"/>"
                "<td class=\"f2\" style=\"width: %.2f%%\"/>"
                "<td class=\"f3\" style=\"width: %.2f%%\"/>"
                "<td class=\"f4\" style=\"width: %.2f%%\"/>"
                "</tr>"
                "</table>"
                "</td>",
        (int) floor(log10(s + f) * 60), f1, f2, ps.in * 100, ps.fs * 100, s, f, f1, f2, f3, f4);
}

void print_pred_name(FILE* fp, int site, int p)
{
    fputs("<td>", fp);

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
    default:
        assert(0);
    }

    fputs("</td>", fp);

    fprintf(fp, "<td>%s</td><td>%s:%d</td>",
        sites[site].fun, sites[site].file, sites[site].line);
}

void process_report(FILE* fp,
                    void (*process_s_site)(int u, int c, int x, int y, int z),
                    void (*process_r_site)(int u, int c, int x, int y, int z),
                    void (*process_b_site)(int u, int c, int x, int y))
{
    int u, c, x, y, z;

    while (1) {
        fscanf(fp, "%d\n", &u);
        if (feof(fp))
            break;
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
        default:
            assert(0);
        }
    }
}

