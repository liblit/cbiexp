#include <cstdio>
#include <cassert>
#include <cmath>

main()
{
    char str[8000];
    char sc;
    int u, c, p;
    float lb, in, fs, co;
    int s, f, os, of;
    char pred[8000];

    while (1) {
	fgets(str, 8000, stdin);
	if (feof(stdin))
	    break;
	sscanf(str, "%c %d %d %d %f %f %f %f %d %d %d %d %[^\n]s", &sc, &u, &c, &p, &lb, &in, &fs, &co, &s, &f, &os, &of, pred);
	float f1 = co * 100;
	float f2 = lb * 100;
	float f3 = (fs - (co + lb)) * 100;
	float f4 = (1.0 - fs) * 100;
	printf("<tr>\n"
	       "<td><table class=\"scores\" style=\"width: %dpx\" title=\"Context: %.0f%% Lower Bound: %.0f%%, Increase: %.0f%%, Fail: %.0f%%, true in %d S and %d F runs\"><tr>"
	       "<td class=\"f1\" style=\"width: %.2f%%\"/>"
	       "<td class=\"f2\" style=\"width: %.2f%%\"/>"
	       "<td class=\"f3\" style=\"width: %.2f%%\"/>"
	       "<td class=\"f4\" style=\"width: %.2f%%\"/>"
	       "</tr></table></td>\n"
	       "<td>%s</td>\n"
	       "</tr>\n", (int) floor(log10(s + f) * 60), f1, f2, in * 100, fs * 100, s, f,
			  f1, f2, f3, f4, pred);
    }
}
