#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include "def.h"

#define CBI_WEBPAGE "http://www.cs.berkeley.edu/~liblit/sampler/"

char* result_summary_file = NULL;
char* prefix = NULL;

void process_cmdline(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-r")) {
            i++;
            result_summary_file = argv[i];
            continue;
        }
        if (!strcmp(argv[i], "-p")) {
            i++;
            prefix = argv[i];
            continue;
        }
        if (!strcmp(argv[i], "-h")) {
            printf("Usage: gen-views -r <result_summary_file> -p <prefix>\n");
            exit(0);
        }
        printf("Illegal option: %s\n", argv[i]);
        exit(1);
    }

    if (!result_summary_file || !prefix) {
        printf("Incorrect usage; try -h\n");
        exit(1);
    }
}

int main(int argc, char** argv)
{
    int i, j, m, n;
    FILE* fp;

    process_cmdline(argc, argv);

    for (i = 0; i < NUM_SCHEMES; i++) {
        for ( m = 0; m < NUM_SORTBYS; m++) {
            char file[1000];
            sprintf(file, "%s%s_%s.html", prefix, scheme_codes[i], sortby_codes[m]);
            fp = fopen(file, "w"); assert(fp);
            fprintf(fp, "<html>\n"
                        "<head><link type=\"text/css\" rel=\"stylesheet\" href=\"http://www.stanford.edu/~mhn/style.css\"/></head>\n"
                        "<body>\n<center><table>\n");
            fprintf(fp, "<tr>\n<td align=right>Scheme:</td><td align=left>");
            for (int j = 0; j < NUM_SCHEMES; j++) {
                if (i == j) 
                    fprintf(fp, "[%s] ", scheme_names[j]);
                else
                    fprintf(fp, "[<a href=\"%s%s_%s.html\">%s</a>] ", prefix, scheme_codes[j], sortby_codes[m], scheme_names[j]);
            }
            fprintf(fp, "</td>\n<td rowSpan=3 align=right>"
                        "<a href=\"%s\">"
                        "<img src=\"http://www.cs.berkeley.edu/~liblit/sampler/logo.png\" style=\"border-style: none\">"
                        "</a></td>\n</tr>\n"
                        "<tr>\n<td align=right>Sorted by:</td><td align=left>", CBI_WEBPAGE);
            for (int n = 0; n < NUM_SORTBYS; n++) {
                if (m == n) 
                    fprintf(fp, "[%s] ", sortby_names[n]);
                else
                    fprintf(fp, "[<a href=\"%s%s_%s.html\">%s</a>] ", prefix, scheme_codes[i], sortby_codes[n], sortby_names[n]);
            }
            fprintf(fp, "</td>\n</tr>\n");
            fprintf(fp, "<tr><td align=right>Go to:</td><td align=left>[<a href=\"%s\">report summary</a>] [<a href=\"%s\">CBI webpage</a>]</td></tr>\n",
                    result_summary_file, CBI_WEBPAGE);
            fprintf(fp, "</center></table>\n<center><table>\n<tr>\n" 
                        "<td></td>\n"
                        "<td align=middle>predicate</td>\n"
                        "<td align=middle>function</td>\n"
                        "<td align=middle>file:line</td>\n"
                        "</tr>\n");
            fclose(fp);

            char cmd[1000];
            sprintf(cmd, "sort -k%d -r -n < %s.txt | /moa/sc3/mhn/bin/format >> %s\n", sortby_indices[m], scheme_codes[i], file);
            system (cmd);
            sprintf(cmd, "echo \"</table></center>\n</body>\n</html>\" >> %s\n", file);
        }
    }

    fp = fopen(result_summary_file, "a");
    assert(fp);

    fprintf(fp, "<table border=1>\n<tr>\n<td>Sorted by:</td>\n");

    for (i = 0; i < NUM_SORTBYS; i++)
        fprintf(fp, "<td align=middle>%s</td>\n", sortby_names[i]);
    fprintf(fp, "</tr>\n");

    for (i = 0; i < NUM_SCHEMES; i++) {
        fprintf(fp, "<tr>\n<td align=left>%s</td>\n", scheme_names[i]);
        for (j = 0; j < NUM_SORTBYS; j++)
            fprintf(fp, "<td align=middle><a href=\"%s%s_%s.html\">X</a></td>\n", prefix, scheme_codes[i], sortby_codes[j]);
        fprintf(fp, "</tr>\n");
    }

    fprintf(fp, "</table>\n");
    fclose (fp);

    return 0;
}

