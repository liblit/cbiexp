#include <stdio.h>
#include <assert.h>
#include "tab.h"

#define NUM_PREDS 15

main()
{
    FILE* cor_fp = fopen("rho.dat", "r");
    assert(cor_fp);

    for (int i = 0; i < NUM_PREDS; i++) {
        char file[1000];
        char s[500000];

        sprintf(file, "r/%d_%d.html", tab[i].site, tab[i].p);
        FILE* ofp = fopen(file, "w");
        assert(ofp);

        fprintf(ofp, "<html>\n<body>\n");
        fprintf(ofp, "%s<br>\n", tab[i].s);

        for (int j = 0; j < NUM_PREDS; j++) {
            float v;
            fscanf(cor_fp, "%f", &v);
            assert(!feof(cor_fp));
            fprintf(ofp, "*** %.2f COR, %s\n", v, tab[j].s);
        }
        fprintf(ofp, "<br>\n");

        fprintf(ofp, "</body>\n</html>\n");
        fclose(ofp);
    }

    fclose(cor_fp);
}

