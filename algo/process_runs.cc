#include <stdio.h>
#include <string.h>
#include <assert.h>

main(int argc, char** argv)
{
    char s[1000], *unit, *scheme, *t;
    char p[1000];

    for (int i = 9157; i <= 25197; i++) {
        if (i == 12750 || i == 17034 || i == 20009 || i == 20833 || i == 21891
         || i == 22258 || i == 24058)
             continue;
        char ifile[1000], ofile[1000];
        sprintf(ifile, "/moa/sc2/cbi/rhythmbox/analyze/ds1/%d.txt", i);
        FILE* ifp = fopen(ifile, "r");
        if (!ifp)
            continue;
        sprintf(ofile, "/moa/sc4/mhn/runs/%d.txt", i);
        FILE* ofp = fopen(ofile, "w");
        assert(ofp);

        fscanf(ifp, "%[^\n]s", s);
        fgetc(ifp);
        assert(strncmp(s, "<rep", 4) == 0);

        while (1) {
            fscanf(ifp, "%[^\n]s", s);
            fgetc(ifp);  // eat '\n'
            if (feof(ifp))
                break;
            if (strncmp(s, "<sam", 4) != 0)
                break;
            unit = strchr(s, '\"'); 
            unit++;
            t = strchr(unit, '\"');
            *t = '\0';
            t++;
            scheme = strchr(t, '\"');
            scheme++;
            t = strchr(scheme, '\"');
            *t = '\0';
            int count = 0;
            while (1) {
                fscanf(ifp, "%[^\n]s", p);
                fgetc(ifp);  // eat '\n'
                if (strncmp(p, "</sam", 5) == 0)
                    break;
                if (strcmp(scheme, "scalar-pairs") == 0)
                    fprintf(ofp, "S");
                else if (strcmp(scheme, "branches") == 0)
                    fprintf(ofp, "B");
                else if (strcmp(scheme, "returns") == 0)
                    fprintf(ofp, "R");
                else
                    assert(0);
                fprintf(ofp, "%s", unit);
                fprintf(ofp, "%05d", count);
                fprintf(ofp, "@ %s\n", p);
                count++;
                assert(count < 99999);
            }
        }
        fclose(ifp);
        fclose(ofp);
    }
    return 0;
}

