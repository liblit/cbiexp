#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <libgen.h>
#include "def.h"
#include "shell.h"
#include "utils.h"

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
	    puts("Usage: gen-views -r <result_summary_file> -p <prefix>");
	    exit(0);
	}
	printf("Illegal option: %s\n", argv[i]);
	exit(1);
    }

    if (!result_summary_file || !prefix) {
	puts("Incorrect usage; try -h");
	exit(1);
    }
}

int main(int argc, char** argv)
{
    int i, j, m;
    FILE* fp;

    process_cmdline(argc, argv);

    for (i = 0; i < NUM_SCHEMES; i++) {
	for ( m = 0; m < NUM_SORTBYS; m++) {

            char file[1000];
	    sprintf(file, "%s_%s.%s.html", scheme_codes[i], sortby_codes[m], prefix);
	    fp = fopen(file, "w");
            assert(fp);

	    fputs("<html>\n"
		  "<head><link type=\"text/css\" rel=\"stylesheet\" href=\"http://www.stanford.edu/~mhn/style.css\"/></head>\n"
		  "<body>\n<center><table>\n"
		  "<tr>\n<td align=right>Scheme:</td><td align=left>",
		  fp);

	    for (int j = 0; j < NUM_SCHEMES; j++) {
		if (i == j)
		    fprintf(fp, "[%s] ", scheme_names[j]);
		else
		    fprintf(fp, "[<a href=\"%s_%s.%s.html\">%s</a>] ", scheme_codes[j], sortby_codes[m], prefix, scheme_names[j]);
	    }
	    fputs("</td>\n<td rowSpan=3 align=right>"
		  "<a href=\"" CBI_WEBPAGE "\">"
		  "<img src=\"http://www.cs.berkeley.edu/~liblit/sampler/logo.png\" style=\"border-style: none\">"
		  "</a></td>\n</tr>\n"
		  "<tr>\n<td align=right>Sorted by:</td><td align=left>",
		  fp);
	    for (int n = 0; n < NUM_SORTBYS; n++) {
		if (m == n)
		    fprintf(fp, "[%s] ", sortby_names[n]);
		else
		    fprintf(fp, "[<a href=\"%s_%s.%s.html\">%s</a>] ", scheme_codes[i], sortby_codes[n], prefix, sortby_names[n]);
	    }
	    fputs("</td>\n</tr>\n", fp);
	    fprintf(fp, "<tr><td align=right>Go to:</td><td align=left>[<a href=\"%s\">report summary</a>] [<a href=\"" CBI_WEBPAGE "\">CBI webpage</a>]</td></tr>\n",
		    result_summary_file);
	    fputs("</center></table>\n<center><table>\n<tr>\n"
		  "<td></td>\n"
		  "<td align=middle>predicate</td>\n"
		  "<td align=middle>function</td>\n"
		  "<td align=middle>file:line</td>\n"
		  "</tr>\n",
		  fp);

            char file2[100];
            sprintf(file2, "%s.tmp.txt", scheme_codes[i]);

	    shell("sort -k%d -r -n < %s.txt > %s", sortby_indices[m], scheme_codes[i], file2);

            FILE* fp2 = fopen(file2, "r");
            assert(fp2);

            while (1) {
                char scheme_code;
                int u, c, p, site, s, f, os, of;
                pred_stat ps;
                fscanf(fp2, "%1s %d %d %d %d %f %f %f %f %d %d %d %d",
                    &scheme_code,
                    &u, &c, &p, &site,
                    &ps.lb, &ps.in, &ps.fs, &ps.co,
                    &s, &f, &os, &of);
                if (feof(fp2))
                    break;
                fputs("<tr>\n", fp);
                print_pred_full(fp, ps, s, f, site, p);
                fputs("</tr>\n", fp);
            }

            fclose(fp2);

	    fputs("</table></center>\n</body>\n</html>\n", fp);

            fclose(fp);
	}
    }

    fp = fopen(result_summary_file, "a");
    assert(fp);

    fputs("<table border=1>\n<tr>\n<td>Sorted by:</td>\n", fp);

    for (i = 0; i < NUM_SORTBYS; i++)
	fprintf(fp, "<td align=middle>%s</td>\n", sortby_names[i]);
    fputs("</tr>\n", fp);

    for (i = 0; i < NUM_SCHEMES; i++) {
	fprintf(fp, "<tr>\n<td align=left>%s</td>\n", scheme_names[i]);
	for (j = 0; j < NUM_SORTBYS; j++)
	    fprintf(fp, "<td align=middle><a href=\"%s_%s.%s.html\">X</a></td>\n", scheme_codes[i], sortby_codes[j], prefix);
	fputs("</tr>\n", fp);
    }

    fputs("</table>\n", fp);
    fclose(fp);

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
