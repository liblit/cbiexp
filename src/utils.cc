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

float compute_lb(int s, int f, int os, int of, int conf)
{
    float fs = ((float)  f) / ( s +  f);
    float co = ((float) of) / (os + of);
    float in = fs - co;
    return in - conf_map[conf - 90] * sqrt(((fs * (1 - fs)) / (f + s)) + ((co * (1 - co))/(of + os)));
}

bool retain_pred(int s, int f, float lb)
{
    return lb > 0 && s + f >= 10;
}

bool retain_pred(int s, int f, int os, int of, int conf)
{
    return retain_pred(s, f, compute_lb(s, f, os, of, conf));
}

const char* s_op[6] = { "<", ">=", "==", "!=", ">", "<=" };
const char* r_op[6] = { "<", ">=", "==", "!=", ">", "<=" };
const char* b_op[2] = { "is FALSE", "is TRUE" };

void print_pred(FILE* fp, int u, int p, int s)
{
    switch (units[u].s[0]) {
    case 'S':
        print_s_pred(fp, p, s);
        break;
    case 'R':
        print_r_pred(fp, p, s);
        break;
    case 'B':
        print_b_pred(fp, p, s);
        break;
    default:
        assert(0);
    }
}

void print_s_pred(FILE* fp, int p, int s)
{
    fprintf(fp, "%s %s %s",
                sites[s].args[0], s_op[p], sites[s].args[1]);
}

void print_r_pred(FILE* fp, int p, int s)
{
    fprintf(fp, "%s %s 0",
                sites[s].args[0], r_op[p]);
}

void print_b_pred(FILE* fp, int p, int s)
{
    fprintf(fp, "%s %s",
                sites[s].args[0], b_op[p]);
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
        switch (units[u].s[0]) {
        case 'S':
            for (c = 0; c < units[u].c; c++) {
                fscanf(fp, "%d %d %d\n", &x, &y, &z);
                assert(x >= 0 && y >= 0 && z >= 0);
                process_s_site(u, c, x, y, z);
            }
            break;
        case 'R':
            for (c = 0; c < units[u].c; c++) {
                fscanf(fp, "%d %d %d\n", &x, &y, &z);
                assert(x >= 0 && y >= 0 && z >= 0);
                process_r_site(u, c, x, y, z);
            }
            break;
        case 'B':
            for (c = 0; c < units[u].c; c++) {
                fscanf(fp, "%d %d\n", &x, &y);
                assert(x >= 0 && y >= 0);
                process_b_site(u, c, x, y);
            }
            break;
        default:
            assert(0);
        }
    }
}

