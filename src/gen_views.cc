#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "def.h"

main(int argc, char** argv)
{
    const char* prefix = (argv[1]) ? argv[1] : "";

    for (int i = 0; i < NUM_SCHEMES; i++) {
        for (int m = 0; m < NUM_SORTBYS; m++) {
            char file[1000];
            sprintf(file, "%s%s_%s.html", prefix, scheme_codes[i], sortby_codes[m]);
            FILE* fp = fopen(file, "w"); assert(fp);
            fprintf(fp, "<html>\n"
                        "<head><link type=\"text/css\" rel=\"stylesheet\" href=\"style.css\"/></head>\n"
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
                    RESULT_SUMMARY_FILE, CBI_WEBPAGE);
            fprintf(fp, "</center></table>\n<center><table>\n<tr>\n" 
                        "<td></td>\n"
                        "<td align=middle>predicate</td>\n"
                        "<td align=middle>function</td>\n"
                        "<td align=middle>file:line</td>\n"
                        "</tr>\n");
            fclose(fp);

            char cmd[1000];
            sprintf(cmd, "sort -k%d -r -n < %s.txt | ../bin/format >> %s\n", sortby_indices[m], scheme_codes[i], file);
            system (cmd);
            sprintf(cmd, "echo \"</table></center>\n</body>\n</html>\" >> %s\n", file);
        }
    }
    return 0;
}

