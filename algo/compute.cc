#include <stdio.h>
#include <assert.h>
#include "param.h"
#include "preds.h"

/*
 * if (i < NUM_SUCCESS_RUNS)
 *    map[i] is the i_th good run specified in file SUCCESS_FILE 
 * else
 *    map[i] is the (NUM_SUCCESS_RUNS - i)_th bad run specified in file FAILURE_FILE
 */
int map[NUM_RUNS];

enum pred_kind {
    ZERO, ONE, BOT, TOP
};

short S0[NUM_PREDS], S1[NUM_PREDS], SB[NUM_PREDS], ST[NUM_PREDS];
short F0[NUM_PREDS], F1[NUM_PREDS], FB[NUM_PREDS], FT[NUM_PREDS];

bool phase1_causes[NUM_PREDS];
bool phase2_causes[NUM_PREDS];
bool is_1_or_T[NUM_FAILURE_RUNS][NUM_PREDS];
bool is_B[NUM_FAILURE_RUNS][NUM_PREDS];

int num_phase1_causes()
{
    int n = 0;
    for (int p = 0; p < NUM_PREDS; p++)
        if (phase1_causes[p])
            n++;
    return n;
}

float Sruns[NUM_PREDS];
float Fruns[NUM_PREDS];

short X[NUM_PREDS];
short Y[NUM_PREDS];
int numX, numY;

inline float rank(int p)
{
    return Sruns[p] / (Sruns[p] + Fruns[p]);
}

void compute_X()
{
    numX = 0;
    for (int p = 0; p < NUM_PREDS; p++) {
        if (!phase1_causes[p]) 
            continue;
        if (!numX) {
            X[0] = p;
            numX = 1;
        } else if (rank(p) < rank(X[0]))
            X[0] = p;
    }
}

void compute_Y()
{
    numY = 0;
    for (int p = 0; p < NUM_PREDS; p++) {
        if (!phase1_causes[p])
            continue;
        int R = NUM_FAILURE_RUNS;
        int R1 = 0;
        int i;
        for (i = 0; i < NUM_FAILURE_RUNS; i++)
            if (is_B[i][p] || is_B[i][X[0]])
                R--;
        for (i = 0; i < NUM_FAILURE_RUNS; i++)
            if (is_1_or_T[i][p] && is_1_or_T[i][X[0]])
                R1++;
        float penalty = 1 - (((float) R1) / R);
        Fruns[p] = Fruns[p] * penalty;
        if (Fruns[p] <= 0) {
            Y[numY] = p;
            numY++;
        }
    }
}

inline void print_ratio(int p, FILE* fp)
{
    fprintf(fp, "%.4f\t", ((float) (S1[p]+ST[p])) / (S1[p]+ST[p]+F1[p]+FT[p])); 
}

inline void print_pred_name(int p, FILE* fp)
{
    fprintf(fp, "%s\n", preds[p]);
}

inline void print_pred_stats(int p, FILE* fp)
{
    fprintf(fp, "S0: %-4d S1: %-4d SB: %-4d ST: %-4d "
                "F0: %-4d F1: %-4d FB: %-4d FT: %-4d\n",
                S0[p], S1[p], SB[p], ST[p],
                F0[p], F1[p], FB[p], FT[p]);
}

inline void print_pred(int p, FILE* fp)
{
    print_pred_name (p, fp);
    print_pred_stats(p, fp);
    fprintf(fp, "\n");
}

inline void inc_B(int i, int p)
{
    if (i < NUM_SUCCESS_RUNS)
        SB[p]++;
    else {
        FB[p]++;
        is_B[i - NUM_SUCCESS_RUNS][p] = true;
    }
}

inline void inc_0(int i, int p)
{
    if (i < NUM_SUCCESS_RUNS)
        S0[p]++;
    else
        F0[p]++;
}

inline void inc_1(int i, int p)
{
    if (i < NUM_SUCCESS_RUNS)
        S1[p]++;
    else {
        F1[p]++;
        is_1_or_T[i - NUM_SUCCESS_RUNS][p] = true;
    }
}

inline void inc_T(int i, int p)
{
    if (i < NUM_SUCCESS_RUNS)
        ST[p]++;
    else {
        FT[p]++;
        is_1_or_T[i - NUM_SUCCESS_RUNS][p] = true;
    }
}


main()
{
    // i, j range over runs; p, q over predicates; s over sites
    int i, j, p, q, s;
    FILE *fp;

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

        sprintf(filename, PATH_PREFIX "/%d.data3", map[i]);
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

        sprintf(filename, PATH_PREFIX "/%d.data2", map[i]);
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
     ** for each predicate, print the number of good and bad runs in which it
     ** is 0, 1, B, T
     ************************************************************************/
 
    fp = fopen(STATS_FILE, "w");
    if (!fp) {
        printf("Error: cannot open file %s for writing\n", STATS_FILE);
        return 1;
    }
    for (p = 0; p < NUM_PREDS; p++) 
        print_pred(p, fp);
    fclose(fp);

    /************************************************************************
     ** compute phase1 set of causes
     ************************************************************************/

    for (p = 0; p < NUM_PREDS; p++)
        phase1_causes[p] = 1;

    for (p = 0; p < NUM_PREDS; p++) {
        assert(S0[p] + S1[p] + SB[p] + ST[p] == NUM_SUCCESS_RUNS);
        assert(F0[p] + F1[p] + FB[p] + FT[p] == NUM_FAILURE_RUNS);

        // if p is not 0 or T in any good run, then it is not a smoking gun
        if (S0[p] == 0 && ST[p] == 0)
            phase1_causes[p] = 0;
        // if p is not 1 or T in any bad  run, then it is not a smoking gun
        if (F1[p] == 0 && FT[p] == 0)
            phase1_causes[p] = 0;
    }
 
    /************************************************************************
     ** print phase1 set of causes
     ************************************************************************/

    fp = fopen(PHASE1_CAUSES_FILE, "w");
    if (!fp) {
        printf("Error: cannot open file %s for writing\n", PHASE1_CAUSES_FILE);
        return 1;
    }
    for (p = 0; p < NUM_PREDS; p++)
        if (phase1_causes[p]) 
            print_pred(p, fp);
    fclose(fp);

    /************************************************************************
     ** compute phase2 set of causes
     ************************************************************************/


    for (p = 0; p < NUM_PREDS; p++) {
        Sruns[p] = S1[p] + ST[p];
        Fruns[p] = F1[p] + FT[p];
    }

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

    /************************************************************************
     ** print phase2 set of causes
     ************************************************************************/

    fp = fopen(PHASE2_CAUSES_FILE, "w");
    if (!fp) {
        printf("Error: cannot open file %s for writing\n", PHASE2_CAUSES_FILE);
        return 1;
    }
    for (p = 0; p < NUM_PREDS; p++)
        if (phase2_causes[p]) 
            print_pred(p, fp);
    fclose(fp);

    return 0;
}

