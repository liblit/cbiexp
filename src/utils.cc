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

pred_info read_pred_full(FILE* fp)
{
    char scheme_code;
    pred_info pi;

    fscanf(fp, "%c %d %d %d %d %f %f %f %f %d %d %d %d",
        &scheme_code,
        &pi.u, &pi.c, &pi.p, &pi.site,
        &pi.ps.lb, &pi.ps.in, &pi.ps.fs, &pi.ps.co,
        &pi.s, &pi.f, &pi.os, &pi.of);

    return pi;
}

void print_pred_full(FILE* fp, pred_info pi)
{
    print_pred_stat(fp, pi);
    print_pred_name(fp, pi.site, pi.p);
}

void print_pred_stat(FILE* fp, pred_info pi)
{
    float f1 = pi.ps.co * 100;
    float f2 = pi.ps.lb * 100;
    float f3 = (pi.ps.fs - (pi.ps.co + pi.ps.lb)) * 100;
    float f4 = (1.0 - pi.ps.fs) * 100;
    fprintf(fp, "<td>"
                "<table class=\"scores\" style=\"width: %dpx\" "
                "title=\"Ctxt: %.0f%% LB: %.0f%%, Incr: %.0f%%, Fail: %.0f%%; tru in %d S and %d F; obs in %d S and %d F\">"
                "<tr>"
                "<td class=\"f1\" style=\"width: %.2f%%\"/>"
                "<td class=\"f2\" style=\"width: %.2f%%\"/>"
                "<td class=\"f3\" style=\"width: %.2f%%\"/>"
                "<td class=\"f4\" style=\"width: %.2f%%\"/>"
                "</tr>"
                "</table>"
                "</td>",
        (int) floor(log10(pi.s + pi.f) * 60), f1, f2, pi.ps.in * 100, pi.ps.fs * 100, pi.s, pi.f, pi.os, pi.of, f1, f2, f3, f4);
}

const char* s_op[6] = { "<", ">=", "==", "!=", ">", "<=" };
const char* r_op[6] = { "<", ">=", "==", "!=", ">", "<=" };
const char* b_op[2] = { "is FALSE", "is TRUE" };
const char* g_op[4] = { "= 0", "= 1", "> 1", "invalid" };

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
    case 'G':
        fprintf(fp, "old_refcount(%s) %s",
            sites[site].args[0], g_op[p]);
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
                    void (*process_b_site)(int u, int c, int x, int y),
                    void (*process_g_site)(int u, int c, int x, int y, int z, int w))
{
    int u, c, x, y, z, w;

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
        case 'G':
            for (c = 0; c < units[u].num_sites; c++) {
                fscanf(fp, "%d %d %d %d\n", &x, &y, &z, &w);
                process_g_site(u, c, x, y, z, w);
            }
            break;
        default:
            assert(0);
        }
    }
}

