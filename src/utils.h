#ifndef UTILS_H
#define UTILS_H

#include <cstdio>

float compute_lb(int s, int f, int os, int of, int conf);
bool retain_pred(int s, int f, float lb);
bool retain_pred(int s, int f, int os, int of, int conf);

void print_pred(FILE* fp, int u, int p, int s);
void print_s_pred(FILE* fp, int p, int s);
void print_r_pred(FILE* fp, int p, int s);
void print_b_pred(FILE* fp, int p, int s);

void process_report(FILE* fp,
                    void (*process_s_site)(int u, int c, int x, int y, int z),
                    void (*process_r_site)(int u, int c, int x, int y, int z),
                    void (*process_b_site)(int u, int c, int x, int y));

#endif
