#include <stdio.h>
#include <assert.h>
#include "def.h"

main(int argc, char** argv)
{
    int i, j;

    const char* prefix = (argv[1]) ? argv[1] : "";

    FILE* fp = fopen(RESULT_SUMMARY_FILE, "a");
    assert(fp);

    fprintf(fp, "<table border=1>\n");

    fprintf(fp, "<tr>\n<td>Sorted by:</td>\n");
    for (i = 0; i < NUM_SORTBYS; i++)
        fprintf(fp, "<td align=middle>%s</td>\n", sortby_names[i]);
    fprintf(fp, "</tr>\n");

    for (i = 0; i < NUM_SCHEMES; i++) {
        fprintf(fp, "<tr>\n<td align=left>%s</td>\n", scheme_names[i]);
        for (j = 0; j < NUM_SORTBYS; j++)
            fprintf(fp, "<td align=middle><a href=\"%s%s_%s.html\">X</a></td>\n", prefix, scheme_codes[i], sortby_codes[j]);
        fprintf(fp, "</tr>\n");
    }
    fprintf(fp, "</table>\n</body></html>\n");
    return 0;
}

