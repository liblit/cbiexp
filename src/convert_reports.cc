#include <cstdio>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include "def.h"
#include "units.h"
#include "classify_runs.h"

int get_indx(char* s)
{
    for (int i = 0; i < NUM_UNITS; i++)
        if (strcmp(s, units[i].s) == 0)
            return i;
    assert(0);
}

main(int argc, char** argv)
{
    char s[3000], *unit, *scheme, *t, u[100];
    char p[3000];

    classify_runs();

    for (int i = 0; i < num_runs; i++) {
        if (!is_srun[i] && !is_frun[i])
            continue;
        char ifile[1000], ofile[1000];

        sprintf(ifile, I_REPORT_PATH_FMT, i);
        FILE* ifp = fopen(ifile, "r");
        assert(ifp);

        sprintf(ofile, O_REPORT_PATH_FMT, i);
        FILE* ofp = fopen(ofile, "w");
        assert(ofp);

        printf("(pass 1) run %d\n", i);

        fgets(s, 3000, ifp);
        assert(strncmp(s, "<rep", 4) == 0);

        while (1) {
            fgets(s, 3000, ifp);
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
                fgets(p, 3000, ifp);
                if (strncmp(p, "</sam", 5) == 0)
                    break;
                fprintf(ofp, "%s", p);
                count++;
            }
            assert(count == units[indx].c);
        }
        fclose(ifp);
        fclose(ofp);
    }
    return 0;
}


/*
        system("rm -f ./reports");
        sprintf(cmd, "gunzip -c /moa/sc2/cbi/rhythmbox/data/%d/client/reports.gz > reports", i);
        system(cmd);
*/
