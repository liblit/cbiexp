#include <stdio.h>
#include <assert.h>
#include "param.h"
#include "preds.h"

short map[NUM_RUNS];

enum pred_kind {
    ZERO, ONE, BOT, TOP
};

short S0[NUM_PREDS], S1[NUM_PREDS], SB[NUM_PREDS], ST[NUM_PREDS];
short F0[NUM_PREDS], F1[NUM_PREDS], FB[NUM_PREDS], FT[NUM_PREDS];

bool** is_0;
bool** is_1;
bool** is_B;
bool** is_T;
bool phase1_causes[NUM_PREDS];
bool phase2_causes[NUM_PREDS];

int num_phase1_causes()
{
    int n = 0;
    for (int p = 0; p < NUM_PREDS; p++)
        if (phase1_causes[p])
            n++;
    return n;
}

inline bool is_less(int p, int q)
{
    int ps = S1[p]+ST[p];
    int qs = S1[q]+ST[q];

    return (ps < qs) || ((ps == qs) && (F1[p]+FT[p] >= F1[q]+FT[q]));
}

short X[NUM_PREDS];
short Y[NUM_PREDS];
int numX, numY;

void compute_X()
{
}

void compute_Y()
{
}

inline void print_ratio(int p, FILE* fp)
{
    fprintf(fp, "%.4f", ((float) (S1[p]+ST[p])) / (S1[p]+ST[p]+F1[p]+FT[p])); 
}

inline void print_pred_name(int p, FILE* fp)
{
    fprintf(fp, "%s", preds[p]);
}

inline void print_pred_stats(int p, FILE* fp)
{
    fprintf(fp, 
                "SB: <a href=\"results/%d.htm#SB\">%d</a> " 
                "S0: <a href=\"results/%d.htm#S0\">%d</a> " 
                "S1: <a href=\"results/%d.htm#S1\">%d</a> " 
                "ST: <a href=\"results/%d.htm#ST\">%d</a> " 
                "FB: <a href=\"results/%d.htm#FB\">%d</a> " 
                "F0: <a href=\"results/%d.htm#F0\">%d</a> " 
                "F1: <a href=\"results/%d.htm#F1\">%d</a> " 
                "FT: <a href=\"results/%d.htm#FT\">%d</a>",
                p, SB[p],
                p, S0[p],
                p, S1[p],
                p, ST[p],
                p, FB[p],
                p, F0[p],
                p, F1[p],
                p, FT[p]);
}

inline void print_pred(int p, FILE* fp)
{
    fprintf(fp, "%-5d$ ", ((S1[p]+ST[p]) * NUM_FAILURE_RUNS) + (NUM_FAILURE_RUNS - (F1[p] + FT[p])));
    fprintf(fp, "%-5d@ ", S1[p]+ST[p]);
    fprintf(fp, "%-5d& ", F1[p]+FT[p]);
    print_pred_name (p, fp);
    fprintf(fp, "<br>");
    print_pred_stats(p, fp);
    fprintf(fp, "<br><br>\n");
}

inline void inc_0(int i, int p)
{
    is_0[i][p] = true;
    if (i < NUM_SUCCESS_RUNS)
        S0[p]++;
    else
        F0[p]++;
}

inline void inc_1(int i, int p)
{
    is_1[i][p] = true;
    if (i < NUM_SUCCESS_RUNS)
        S1[p]++;
    else
        F1[p]++;
}

inline void inc_B(int i, int p)
{
    is_B[i][p] = true;
    if (i < NUM_SUCCESS_RUNS)
        SB[p]++;
    else
        FB[p]++;
}

inline void inc_T(int i, int p)
{
    is_T[i][p] = true;
    if (i < NUM_SUCCESS_RUNS)
        ST[p]++;
    else
        FT[p]++;
}


main()
{
    // i, j range over runs; p, q over predicates; s over sites
    int i, j, p, q, s;
    FILE *fp;

    is_0 = new (bool*)[NUM_RUNS];
    assert(is_0);
    is_1 = new (bool*)[NUM_RUNS];
    assert(is_1);
    is_B = new (bool*)[NUM_RUNS];
    assert(is_B);
    is_T = new (bool*)[NUM_RUNS];
    assert(is_T);
    for (i = 0; i < NUM_RUNS; i++) {
        is_0[i] = new bool [NUM_PREDS];
        assert(is_0[i]);
        is_1[i] = new bool [NUM_PREDS];
        assert(is_1[i]);
        is_B[i] = new bool [NUM_PREDS];
        assert(is_B[i]);
        is_T[i] = new bool [NUM_PREDS];
        assert(is_T[i]);
    }

    /************************************************************************
     ** create map
     ************************************************************************/

    FILE *fps = fopen(SUCCESS_FILE, "r");
    if (!fps) {
        printf("Error: cannot open file %s for reading\n", SUCCESS_FILE);
        return 1;
    }
    FILE *fpf = fopen(FAILURE_FILE, "r");
    if (!fpf) {
        printf("Error: cannot open file %s for reading\n", FAILURE_FILE);
        return 1;
    }
    for (i = 0; i < NUM_SUCCESS_RUNS; i++)
        fscanf(fps, "%d", &map[i]);
    for (; i < NUM_RUNS; i++)
        fscanf(fpf, "%d", &map[i]);

    /************************************************************************
     ** for each predicate, compute number of good and bad runs in which it
     ** is 0, 1, B, T
     ************************************************************************/

    for (i = 0; i < NUM_RUNS; i++) {
        char filename[300];
        p = 0;

        sprintf(filename, DATA_DIR "/%d.data3", map[i]);
        fp = fopen(filename, "r");
        if (!fp) {
            printf("Error: cannot read file %s\n", filename);
            return 1;
        }
        for (s = 0; s < NUM_3_SITES; s++) {
            int x, y, z;
            fscanf(fp, "%d %d %d", &x, &y, &z);
            int num_times_exec = x + y + z;
            if (num_times_exec == 0) {
                inc_B(i, p);
                inc_B(i, p + 1);
                inc_B(i, p + 2);
            } else {
                if (!x)
                    inc_0(i, p);
                else if (x == num_times_exec)
                    inc_1(i, p);
                else
                    inc_T(i, p);
                if (!y)
                    inc_0(i, p + 1);
                else if (y == num_times_exec)
                    inc_1(i, p + 1);
                else
                    inc_T(i, p + 1);
                if (!z)
                    inc_0(i, p + 2);
                else if (z == num_times_exec)
                    inc_1(i, p + 2);
                else
                    inc_T(i, p + 2);
            }
            p += 3;
        }
        fclose(fp);

        sprintf(filename, DATA_DIR "/%d.data2", map[i]);
        fp = fopen(filename, "r");
        if (!fp) {
            printf("Error: cannot open file %s for reading\n", filename);
            return 1;
        }
        for (s = 0; s < NUM_2_SITES; s++) {
            int x, y;
            fscanf(fp, "%d %d", &x, &y);
            int num_times_exec = x + y;
            if (num_times_exec == 0) {
                inc_B(i, p);
                inc_B(i, p + 1);
            } else {
                if (!x)
                    inc_0(i, p);
                else if (x == num_times_exec)
                    inc_1(i, p);
                else
                    inc_T(i, p);
                if (!y)
                    inc_0(i, p + 1);
                else if (y == num_times_exec)
                    inc_1(i, p + 1);
                else
                    inc_T(i, p + 1);
            }
            p += 2;
        }
        fclose(fp);
    }

    /************************************************************************
     ** compute phase1 set of causes
     ************************************************************************/

    for (p = 0; p < NUM_PREDS; p++)
        phase1_causes[p] = 1;

    for (p = 0; p < NUM_PREDS; p++) {
        // if p is not 0 or T in any good run, then it is not a smoking gun
        if (S0[p] == 0 && ST[p] == 0)
            phase1_causes[p] = 0;
        // if p is not 1 or T in any bad  run, then it is not a smoking gun
        else if (F1[p] == 0 && FT[p] == 0)
            phase1_causes[p] = 0;
    }
 
    /************************************************************************
     ** print phase1 set of causes
     ************************************************************************/

    for (p = 0; p < NUM_PREDS; p++) {
        if (!phase1_causes[p])
            continue;
        char filename[100];
        sprintf(filename, "%d.htm", p);
        fp = fopen(filename, "w");
        if (!fp) {
            printf("Error: cannot open file %s for writing\n", filename);
            return 1;
        }
        fprintf(fp, "<html>\n");
        fprintf(fp, "<body>\n");
        fprintf(fp, "<title>%s</title>\n", preds[p]); 

        int print_count;

        fprintf(fp, "<b><a name=\"SB\">SB[%d]:</a></b> ", SB[p]);
        for (i = 0, print_count = 0; i < NUM_SUCCESS_RUNS; i++) 
            if (is_B[i][p]) {
               if (print_count >= MAX_PRINT_COUNT) {
                   fprintf(fp, "...");
                   break;
               }
               fprintf(fp, "<a href=\"../data9/%d\">%d</a> ", map[i], map[i]);
               print_count++;
            }
        fprintf(fp, "<br>\n");

        fprintf(fp, "<b><a name=\"S0\">S0[%d]:</a></b> ", S0[p]);
        for (i = 0, print_count = 0; i < NUM_SUCCESS_RUNS; i++)
            if (is_0[i][p]) {
               if (print_count >= MAX_PRINT_COUNT) {
                   fprintf(fp, "...");
                   break;
               }
               fprintf(fp, "<a href=\"../data9/%d\">%d</a> ", map[i], map[i]);
               print_count++;
            }
        fprintf(fp, "<br>\n");

        fprintf(fp, "<b><a name=\"S1\">S1[%d]:</a></b> ", S1[p]);
        for (i = 0, print_count = 0; i < NUM_SUCCESS_RUNS; i++)
            if (is_1[i][p]) {
               if (print_count >= MAX_PRINT_COUNT) {
                   fprintf(fp, "...");
                   break;
               }
               fprintf(fp, "<a href=\"../data9/%d\">%d</a> ", map[i], map[i]);
               print_count++;
            }
        fprintf(fp, "<br>\n");

        fprintf(fp, "<b><a name=\"ST\">ST[%d]:</a></b> ", ST[p]);
        for (i = 0, print_count = 0; i < NUM_SUCCESS_RUNS; i++)
            if (is_T[i][p]) {
               if (print_count >= MAX_PRINT_COUNT) {
                   fprintf(fp, "...");
                   break;
               }
               fprintf(fp, "<a href=\"../data9/%d\">%d</a> ", map[i], map[i]);
               print_count++;
            }
        fprintf(fp, "<br>\n");

        fprintf(fp, "<b><a name=\"FB\">FB[%d]:</a></b> ", FB[p]);
        for (i = NUM_SUCCESS_RUNS, print_count = 0; i < NUM_RUNS; i++)
            if (is_B[i][p]) {
               if (print_count >= MAX_PRINT_COUNT) {
                   fprintf(fp, "...");
                   break;
               }
               fprintf(fp, "<a href=\"../data9/%d\">%d</a> ", map[i], map[i]);
               print_count++;
            }
        fprintf(fp, "<br>\n");

        fprintf(fp, "<b><a name=\"F0\">F0[%d]:</a></b> ", F0[p]);
        for (i = NUM_SUCCESS_RUNS, print_count = 0; i < NUM_RUNS; i++)
            if (is_0[i][p]) {
               if (print_count >= MAX_PRINT_COUNT) {
                   fprintf(fp, "...");
                   break;
               }
               fprintf(fp, "<a href=\"../data9/%d\">%d</a> ", map[i], map[i]);
               print_count++;
            }
        fprintf(fp, "<br>\n");

        fprintf(fp, "<b><a name=\"F1\">F1[%d]:</a></b> ", F1[p]);
        for (i = NUM_SUCCESS_RUNS, print_count = 0; i < NUM_RUNS; i++)
            if (is_1[i][p]) {
               if (print_count >= MAX_PRINT_COUNT) {
                   fprintf(fp, "...");
                   break;
               }
               fprintf(fp, "<a href=\"../data9/%d\">%d</a> ", map[i], map[i]);
               print_count++;
            }
        fprintf(fp, "<br>\n");

        fprintf(fp, "<b><a name=\"FT\">FT[%d]:</a></b> ", FT[p]);
        for (i = NUM_SUCCESS_RUNS, print_count = 0; i < NUM_RUNS; i++)
            if (is_T[i][p]) {
               if (print_count >= MAX_PRINT_COUNT) {
                   fprintf(fp, "...");
                   break;
               }
               fprintf(fp, "<a href=\"../data9/%d\">%d</a> ", map[i], map[i]);
               print_count++;
            }
        fprintf(fp, "<br>\n");

        fprintf(fp, "</body>\n");
        fprintf(fp, "</html>\n");
        fclose(fp);
    }       


    fp = fopen(PHASE1_CAUSES_FILE, "w");
    if (!fp) {
        printf("Error: cannot open file %s for writing\n", PHASE1_CAUSES_FILE);
        return 1;
    }
    fprintf(fp, "<html>\n");
    fprintf(fp, "<body>\n");

    for (p = 0; p < NUM_PREDS; p++)
        if (phase1_causes[p]) 
            print_pred(p, fp);

    fprintf(fp, "</body>\n");
    fprintf(fp, "</html>\n");
    fclose(fp);

    /************************************************************************
     ** compute phase2 set of causes
     ************************************************************************/

/*
    while (num_phase1_causes()) {
        compute_X();
        for (i = 0; i < numX; i++) {
            phase1_causes[X[i]] = 0;
            phase2_causes[X[i]] = 1;
        }
        compute_Y();
        for (i = 0; i < numY; i++)
            phase1_causes[Y[i]] = 0;
    }
*/

    /************************************************************************
     ** print phase2 set of causes
     ************************************************************************/

/*
    fp = fopen(PHASE2_CAUSES_FILE, "w");
    if (!fp) {
        printf("Error: cannot open file %s for writing\n", PHASE2_CAUSES_FILE);
        return 1;
    }
    for (p = 0; p < NUM_PREDS; p++)
        if (phase2_causes[p]) 
            print_pred(p, fp);
    fclose(fp);
*/

    return 0;
}

