#ifndef SCAFFOLD_H
#define SCAFFOLD_H

#include <cstdio>


void scaffold(const char [],
              void (*process_s_site)(int r, int u, int c, int x, int y, int z),
              void (*process_r_site)(int r, int u, int c, int x, int y, int z),
              void (*process_b_site)(int r, int u, int c, int x, int y));


#endif
