#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "def.h"
#include "units.h"

bool is_srun[NUM_RUNS];
bool is_frun[NUM_RUNS];

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
    FILE *sfp = NULL, *ffp = NULL;
    int i;
                                                                                                                                                                                                     
    /************************************************************************
     ** process command line options
     ************************************************************************/

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) {
            if (sfp) {
                printf("-s specified multiple times\n");
                return 1;
            }
            i++;
            if (!argv[i]) {
                printf("argument to -s missing\n");
                return 1;
            }
            sfp = fopen(argv[i], "r");
            if (!sfp) {
                printf("Cannot open file %s for reading\n", argv[i]);
                return 1;
            }
            continue;
        }
        if (strcmp(argv[i], "-f") == 0) {
            if (ffp) {
                printf("-f specified multiple times\n");
                return 1;
            }
            i++;
            if (!argv[i]) {
                printf("argument to -f missing\n");
                return 1;
            }
            ffp = fopen(argv[i], "r");
            if (!ffp) {
                printf("Cannot open file %s for reading\n", argv[i]);
                return 1;
            }
            continue;
        }
        if (strcmp(argv[i], "-h") == 0) {
            printf("Usage: process_runs -s runs_file -f runs_file\n");
            return 0;
        }
        printf("Illegal option: %s\n", argv[i]);
        return 1;
    }

    if (!sfp || !ffp) {
        printf("Incorrect usage; try -h\n");
        return 1;
    }

    /************************************************************************
     ** read -s specified file
     ************************************************************************/
                                                                                                                                                                                                     
    while (1) {
        fscanf(sfp, "%d", &i);
        if (feof(sfp))
            break;
        if (i < 0 || i >= NUM_RUNS) {
            printf("Illegal run %d in -s specified file\n", i);
            return 1;
        }
        is_srun[i] = true;
    }
    fclose(sfp);
                                                                                                                                                                                                     
    /************************************************************************
     ** read -f specified file 
     ************************************************************************/
                                                                                                                                                                                                     
    while (1) {
        fscanf(ffp, "%d", &i);
        if (feof(ffp))
            break;
        if (i < 0 || i >= NUM_RUNS) {
            printf("Illegal run %d in -f specified file\n", i);
            return 1;
        }
        is_frun[i] = true;
    }
    fclose(ffp);
                                                                                                                                                                                                     
    /************************************************************************
     ** do sanity check (no run should be both successful and failing)
     ************************************************************************/
                                                                                                                                                                                                     
    int ns = 0, nf = 0;
    for (i = 0; i < NUM_RUNS; i++) {  
        if (is_srun[i] && is_frun[i]) {
            printf("Run %d is both successful and failing\n", i);
            return 1;
        }
        if (is_srun[i])
            ns++;
        else if (is_frun[i])
            nf++;
    }
    printf("#S runs: %d #F runs: %d\n", ns, nf);

    /************************************************************************
     ** main loop
     ************************************************************************/

    for (i = 0; i < NUM_RUNS; i++) {
        if (!is_srun[i] && !is_frun[i])
            continue;

        char ifile[1000], ofile[1000];

        printf("r %d\n", i);

        sprintf(ifile, "/moa/sc2/cbi/rhythmbox/analyze/ds1/%d.txt", i);
        FILE* ifp = fopen(ifile, "r");
        assert(ifp);

        sprintf(ofile, "/moa/sc4/mhn/rb/base/%d.txt", i);
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

/*
        system("rm -f ./reports");
        sprintf(cmd, "gunzip -c /moa/sc2/cbi/rhythmbox/data/%d/client/reports.gz > reports", i);
        system(cmd);
*/
