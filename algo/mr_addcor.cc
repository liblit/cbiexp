#include <stdio.h>
#include <assert.h>
#include "tab.h"

#define NUM_PREDS 861

main()
{
    FILE* cor_fp = fopen("rho.dat", "r");
    assert(cor_fp);

    for (int i = 0; i < NUM_PREDS; i++) {
        char file[1000];
        char s[500000];

        sprintf(file, "r/%d_%d_%d.html", tab[i].u, tab[i].c, tab[i].p);
        FILE* ifp = fopen(file, "r");
        assert(ifp);
        fscanf(ifp, "%[^\n]s", s); fgetc(ifp); // eat <html>
        fscanf(ifp, "%[^\n]s", s); fgetc(ifp); // eat <body>
        fscanf(ifp, "%[^\n]s", s); fgetc(ifp); // eat predicate name
        fscanf(ifp, "%[^\n]s", s); fgetc(ifp); // eat <br><br>

        sprintf(file, "r/%d_%d_%d.htm", tab[i].u, tab[i].c, tab[i].p);
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

        fscanf(ifp, "%[^\n]s", s); fgetc(ifp); // read F: ...
        fprintf(ofp, "%s\n", s);
        fscanf(ifp, "%[^\n]s", s); fgetc(ifp); // read <br><br>
        fprintf(ofp, "%s\n", s);
        fscanf(ifp, "%[^\n]s", s);             // read S: ...
        fprintf(ofp, "%s\n", s);
        fprintf(ofp, "</body>\n</html>\n");
        fclose(ifp);
        fclose(ofp);
    }

    fclose(cor_fp);
}

