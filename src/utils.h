#ifndef UTILS_H
#define UTILS_H

#include <cstdio>

struct pred_stat {
    float lb;
    float in;
    float fs;
    float co;
};

struct pred_info {
    int u, c, p, site;
    pred_stat ps;
    int s, f, os, of;
};

pred_stat compute_pred_stat(int s, int f, int os, int of, int confidence);

bool retain_pred(int s, int f, float lb);
bool retain_pred(int s, int f, int os, int of, int conf);

pred_info read_pred_full(FILE* fp);
void print_pred_full(FILE* fp, pred_info pi);
void print_pred_stat(FILE* fp, pred_info pi);
void print_pred_name(FILE* fp, int site, int p);

void process_report(FILE* fp,
                    void (*process_s_site)(int u, int c, int x, int y, int z),
                    void (*process_r_site)(int u, int c, int x, int y, int z),
                    void (*process_b_site)(int u, int c, int x, int y),
                    void (*process_g_site)(int u, int c, int x, int y, int z, int w));

#endif
