#ifndef UTILS_H
#define UTILS_H

float compute_lb(int s, int f, int os, int of, int conf);
bool retain_pred(int s, int f, float lb);
bool retain_pred(int s, int f, int os, int of, int conf);

#endif
