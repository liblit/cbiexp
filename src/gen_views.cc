#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define NUM_SCHEMES 4
#define NUM_SORTBYS 4

char* scheme_codes[NUM_SCHEMES] = { "B", "R", "S", "preds" };
char* scheme_names[NUM_SCHEMES] = { "branch", "return", "scalar", "all" };
char* sortby_codes[NUM_SORTBYS] = { "lb", "is", "fs", "nf" };
char* sortby_names[NUM_SORTBYS] = { "lower bound of confidence interval", "increase score", "fail score", "true in # F runs" };
int   sortby_indices[NUM_SORTBYS] = { 5, 6, 7, 10 };


main(int argc, char** argv)
{
    const char* prefix = (argv[1]) ? argv[1] : "";

    for (int i = 0; i < NUM_SCHEMES; i++) {
        for (int m = 0; m < NUM_SORTBYS; m++) {
            char file[1000];
            sprintf(file, "%s_%s_%s.html", prefix, scheme_codes[i], sortby_codes[m]);
            FILE* fp = fopen(file, "w"); assert(fp);
            fprintf(fp, "<html>\n"
                        "<head><link type=\"text/css\" rel=\"stylesheet\" href=\"style.css\"/></head>\n"
                        "<body>\n<center><table>\n");
            fprintf(fp, "<tr>\n<td align=middle>Scheme: ");
            for (int j = 0; j < NUM_SCHEMES; j++) {
                if (i == j) 
                    fprintf(fp, "[%s] ", scheme_names[j]);
                else
                    fprintf(fp, "[<a href=\"%s_%s_%s.html\">%s</a>] ", prefix, scheme_codes[j], sortby_codes[m], scheme_names[j]);
            }
            fprintf(fp, "</td>\n<td rowSpan=2 align=right>"
                        "<a href=\"http://www.cs.berkeley.edu/~liblit/sampler/\">"
                        "<img src=\"http://www.cs.berkeley.edu/~liblit/sampler/logo.png\" style=\"border-style: none\">"
                        "</a></td>\n</tr>\n"
                        "<tr>\n<td align=middle>Sort by: ");
            for (int n = 0; n < NUM_SORTBYS; n++) {
                if (m == n) 
                    fprintf(fp, "[%s] ", sortby_names[n]);
                else
                    fprintf(fp, "[<a href=\"%s_%s_%s.html\">%s</a>] ", prefix, scheme_codes[i], sortby_codes[n], sortby_names[n]);
            }
            fprintf(fp, "</td>\n</tr>\n</center></table>\n<table>\n<tr>\n" 
                        "<td></td>\n"
                        "<td align=middle>predicate</td>\n"
                        "<td align=middle>function</td>\n"
                        "<td align=middle>file:line</td>\n"
                        "</tr>\n");
            fclose(fp);

            char cmd[1000];
            sprintf(cmd, "sort -k%d -r -n < %s.txt | ./format >> %s\n", sortby_indices[m], scheme_codes[i], file);
            system (cmd);
            sprintf(cmd, "echo \"</table>\n</body>\n</html>\" >> %s\n", file);
        }
    }
    return 0;
}

