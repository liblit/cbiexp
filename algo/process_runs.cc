#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "def.h"
#include "units.h"

int get_indx(char* s)
{
    for (int i = 0; i < NUM_UNITS; i++)
        if (strcmp(s, units[i].s) == 0)
            return i;
    assert(0);
}

main(int argc, char** argv)
{
    char s[1000], *unit, *scheme, *t, u[100];
    char p[1000];

    for (int i = 1; i <= NUM_RUNS; i++) {
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

            if (strcmp(scheme, "scalar-pairs") == 0)
                sprintf(u, "S%s", unit);
            else if (strcmp(scheme, "branches") == 0)
                sprintf(u, "B%s", unit);
            else if (strcmp(scheme, "returns") == 0)
                sprintf(u, "R%s", unit);
            else
                assert(0);

            int indx = get_indx(u);
            fprintf(ofp, "%d\n", indx);

            int count = 0;
            while (1) {
                fscanf(ifp, "%[^\n]s", p);
                fgetc(ifp);  // eat '\n'
                if (strncmp(p, "</sam", 5) == 0)
                    break;
                fprintf(ofp, "%s\n", p);
                count++;
            }
            assert(count == units[indx].c);
        }
        fclose(ifp);
        fclose(ofp);
    }
    return 0;
}

