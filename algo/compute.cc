#include <stdio.h>
#include <assert.h>
#include <string.h>

#define NUM_S_SITES 133395
#define NUM_R_SITES 8429
#define NUM_B_SITES 3418
#define NUM_SITES   (NUM_S_SITES + NUM_R_SITES + NUM_B_SITES)  // 857780
#define NUM_PREDS   (NUM_S_SITES * 6) + (NUM_R_SITES * 6) + (NUM_B_SITES * 2)
#define NUM_RUNS    20000

FILE* out;

bool is_srun[NUM_RUNS + 1];
bool is_frun[NUM_RUNS + 1];

int S[NUM_PREDS], F[NUM_PREDS], OS[NUM_PREDS], OF[NUM_PREDS];

inline void inc(int p, int i)
{ 
    if (is_srun[i]) 
        S[p]++; 
    else if (is_frun[i])
        F[p]++; 
}

inline void obs(int p, int i) 
{ 
    if (is_srun[i]) 
        OS[p]++; 
    else if (is_frun[i]) 
        OF[p]++; 
}

inline void print_pred(int p, char* pred_kind, char* site_name, char* full_name)
{
    float crash   = ((float)  F[p]) / ( S[p] +  F[p]);
    float context = ((float) OF[p]) / (OS[p] + OF[p]);
    fprintf(out, "%.2f In %.2f Cr %.2f Co %d S %d F %d OS %d OF ", crash - context, crash, context, S[p], F[p], OS[p], OF[p]);
    fprintf(out, "<a href=\"r/%s_%s.html\">", pred_kind, site_name);
    fprintf(out, "%s %s", pred_kind, full_name);
    fprintf(out, "</a><br>\n");
}

main(int argc, char** argv)
{
    int i, p, s, x, y, z;
    FILE *sfp = NULL, *ffp = NULL, *ifp = NULL, *fps;
    char line_s[2000], *at;

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
        if (strcmp(argv[i], "-i") == 0) {
            if (ifp) {
                printf("-i specified multiple times\n");
                return 1;
            }
            i++;
            if (!argv[i]) {
                printf("argument to -i missing\n");
                return 1;
            }
            ifp = fopen(argv[i], "r");
            if (!ifp) {
                printf("Cannot open file %s for reading\n", argv[i]);
                return 1;
            }
            continue;
        }
        if (strcmp(argv[i], "-h") == 0) {
            printf("Usage: compute [-i runs_file] [-s runs_file] [-f runs_file]\n");
            return 0;
        }
        printf("Illegal option: %s\n", argv[i]);
        return 1;
    }

    /************************************************************************
     ** read -s specified file (if any)
     ************************************************************************/

    if (sfp) {
        while (1) {
            fscanf(sfp, "%d", &i);
            if (feof(sfp))
                break;
            if (i <= 0 || i > NUM_RUNS) {
                printf("Illegal run %d in -s specified file\n", i);
                return 1;
            }
            is_srun[i] = true;
        }
        fclose(sfp);
    }

    /************************************************************************
     ** read -f specified file (if any)
     ************************************************************************/

    if (ffp) {
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
    }

    /************************************************************************
     ** read -i specified file (if any)
     ************************************************************************/

    if (ifp) {
        while (1) {
            fscanf(ifp, "%d", &i);
            if (feof(ifp))
                break;
            if (i <= 0 || i > NUM_RUNS) {
                printf("Illegal run %d in -i specified file\n", i);
                return 1;
            }
            if (is_srun[i] && is_frun[i])
                printf("Warning: Run %d is present in both -s and -f specified files but is being ignored since it is present in -i specified file as well\n");
            is_srun[i] = false;
            is_frun[i] = false;
        }
        fclose(ifp);
    }

    /************************************************************************
     ** do sanity check (no run should be both successful and failing)
     ************************************************************************/

    int ns = 0, nf = 0;
    for (i = 1; i <= NUM_RUNS; i++) {
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
     ** compute for each predicate, number of successful and failing runs in
     ** which it is (1) just observed, and (2) observed to be true.
     ************************************************************************/

//#undef NUM_RUNS
//#define NUM_RUNS 2

    for (i = 1; i <= NUM_RUNS; i++) {
        if (!is_srun[i] && !is_frun[i])
            continue;
        printf("r %d\n", i);
        char file_r[1000], line_r[2000];
        if (i <= 9156)
            sprintf(file_r, "/moa/sc3/mhn/runs/%d.txt", i);
        else
            sprintf(file_r, "/moa/sc4/mhn/runs/%d.txt", i);
        FILE* fpr = fopen(file_r, "r");
        assert(fpr);
        fps = fopen("sites.txt", "r");
        bool active = false;

        for (s = 0, p = 0; s < NUM_SITES; s++) {
            fscanf(fps, "%[^\n]s", line_s);
            fgetc(fps);  
            if (!active) {
                fscanf(fpr, "%[^\n]s", line_r);
                fgetc(fpr);  
                if (feof(fpr)) break;
            }
            char site_kind = line_s[0];
            assert(site_kind == 'S' || site_kind == 'R' || site_kind == 'B');
            if (strncmp(line_s, line_r, 38) == 0) {
                active = false;
                at = strchr(line_r, '@');
                at += 2;
                if (site_kind == 'S' || site_kind == 'R') {
                    sscanf(at, "%d %d %d", &x, &y, &z); 
                    if (x + y + z > 0) {
                        obs(p  , i);
                        obs(p+1, i);
                        obs(p+2, i);
                        obs(p+3, i);
                        obs(p+4, i);
                        obs(p+5, i);
                    }
                    if (x     > 0) inc(p  , i);
                    if (y + z > 0) inc(p+1, i);
                    if (y     > 0) inc(p+2, i);
                    if (x + z > 0) inc(p+3, i);
                    if (z     > 0) inc(p+4, i);
                    if (x + y > 0) inc(p+5, i);
                } else {
                    sscanf(at, "%d %d", &x, &y); 
                    if (x + y > 0) {
                        obs(p  , i);
                        obs(p+1, i);
                    }
                    if (x > 0) inc(p  , i);
                    if (y > 0) inc(p+1, i);
                }
            } else 
                active = true;
            if (site_kind == 'S' || site_kind == 'R')
                p += 6;
            else
                p += 2;
        }
        fclose(fpr);
        fclose(fps);
    }

    out = fopen("out.html", "w");
    assert(out);

    fps = fopen("sites.txt", "r");
    fprintf(out, "<html>\n<body>\n");
    for (s = 0, p = 0; s < NUM_SITES; s++) {
        fscanf(fps, "%[^\n]s", line_s);
        fgetc(fps); 
        at = strchr(line_s, '@');
        *at = '\0';
        at += 2;
        switch (line_s[0]) {
        case 'S':
        case 'R':
           print_pred(p  , "LT" , line_s, at);
           print_pred(p+1, "NLT", line_s, at);
           print_pred(p+2, "EQ" , line_s, at);
           print_pred(p+3, "NEQ", line_s, at);
           print_pred(p+4, "GT" , line_s, at);
           print_pred(p+5, "NGT", line_s, at);
           p += 6;
           break;
        case 'B':
           print_pred(p  , "F", line_s, at);
           print_pred(p+1, "T", line_s, at);
           p += 2;
           break;
        default:
           assert(0);
        }
    }
    fprintf(out, "</html>\n</body>\n");
    fclose(fps);
    fclose(out);

    return 0;
}

