#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>


#define NUM_S_SITES 133395
#define NUM_R_SITES 8429
#define NUM_B_SITES 3418
#define NUM_SITES   (NUM_S_SITES + NUM_R_SITES + NUM_B_SITES)  
#define NUM_PREDS   (NUM_S_SITES * 6) + (NUM_R_SITES * 6) + (NUM_B_SITES * 2)
#define NUM_RUNS    25197

int num_runs_needed;

bool is_srun[NUM_RUNS + 1];
bool is_frun[NUM_RUNS + 1];

int runs[20];
int num_runs_found = 0;

main(int argc, char** argv)
{
    int i, s, x, y, z;
    FILE *ffp = NULL;
    char* site_name = NULL;
    char* pred_kind = NULL;
    int p;

    /************************************************************************
     ** process command line options
     ************************************************************************/

    if (argc != 4) {
        printf("usage: find_runs N pred_kind site_name\n");
        printf("N = Number of failed runs to find\n");
        printf("pred_kind = T|F|LT|NLT|EQ|NEQ|GT|NGT\n");
        printf("site_name = 38-char string beginning with B|R|S\n");
        return 1;
    }

    num_runs_needed = atoi(argv[1]);

    pred_kind = argv[2];

    switch (argv[2][0]) {
    case 'F':
         p = 0; break;
    case 'T':
         p = 1; break;
    case 'L':
         p = 0; break;
    case 'E':
         p = 2; break;
    case 'G':
         p = 4; break;
    case 'N':
         switch (argv[2][1]) {
         case 'L': p = 1; break;
         case 'E': p = 3; break;
         case 'G': p = 5; break;
         default: assert(0);
         }
         break;
    default: assert(0);
    }

    site_name = argv[3];

    printf("Searching failed runs in which predicate \"%s %s\" is observed to be true.\n", pred_kind, site_name);
    printf("Will stop after finding at most %d failed runs.\n", num_runs_needed);


    ffp = fopen("f.runs", "r");
    assert(ffp);

    while (1) {
        fscanf(ffp, "%d", &i);
        if (feof(ffp))
            break;
        if (i <= 0 || i > NUM_RUNS) {
            printf("Illegal run %d in -f specified file\n", i);
            return 1;
        }
        is_frun[i] = true;
    }
    fclose(ffp);

    /************************************************************************
     ** compute for each predicate, number of successful and failing runs in
     ** which it is (1) just observed, and (2) observed to be true.
     ************************************************************************/

    for (i = 1; i <= NUM_RUNS; i++) {
        if (num_runs_found >= num_runs_needed)
            break;
        if (!is_frun[i])
            continue;
        char file_r[1000], line_r[1000];

        if (i <= 9156)
            sprintf(file_r, "/moa/sc3/mhn/runs/%d.txt", i);
        else
            sprintf(file_r, "/moa/sc4/mhn/runs/%d.txt", i);
        FILE* fpr = fopen(file_r, "r");
        assert(fpr);

        for (s = 0; s < NUM_SITES; s++) {
            fscanf(fpr, "%[^\n]s", line_r);
            fgetc(fpr);
            if (feof(fpr)) break;
            char site_kind = line_r[0];
            if (strncmp(line_r, site_name, 38) == 0) {
                char* at = strchr(line_r, '@');
                at += 2;
                if (site_kind == 'S' || site_kind == 'R') {
                    sscanf(at, "%d %d %d", &x, &y, &z); 
                    if (x + y + z > 0 && ((p == 0 && x > 0) ||
                                          (p == 1 && y + z > 0) ||
                                          (p == 2 && y > 0) ||
                                          (p == 3 && x + z > 0) ||
                                          (p == 4 && z > 0) ||
                                          (p == 5 && x + y > 0))) {
                        printf("found run: %d\n", i);
                        runs[num_runs_found] = i;
                        num_runs_found++;
                    }
                } else {
                    sscanf(at, "%d %d", &x, &y); 
                    if (x + y > 0 && ((p == 0 && x > 0) || (p == 1 && y > 0))) {
                        printf("found run: %d\n", i);
                        runs[num_runs_found] = i;
                        num_runs_found++;
                    }
                }
                break;
            }
        }
        fclose(fpr);
    }

    char file[1000];
    sprintf(file, "/moa/sc3/mhn/results/%s_%s.html", pred_kind, site_name);
    FILE* fp = fopen(file, "w");
    assert(fp);
    fprintf(fp, "<html>\n<body>\n");
    for (i = 0; i < num_runs_found; i++)
        fprintf(fp, "[<a href=\"/moa/sc2/cbi/rhythmbox/data/%d\">%d</a>] ", runs[i], runs[i]);
    fprintf(fp, "\n</html>\n</body>\n");
    fclose(fp);

    return 0;
}

