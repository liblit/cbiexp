#include <stdio.h>
#include <assert.h>
#include "param.h"

int map[NUM_RUNS];
enum pred_kind {
    ZERO, ONE, BOT, TOP
};
int S0[NUM_PREDS], S1[NUM_PREDS], SB[NUM_PREDS], ST[NUM_PREDS];
int F0[NUM_PREDS], F1[NUM_PREDS], FB[NUM_PREDS], FT[NUM_PREDS];
bool causes[NUM_PREDS];

FILE *out_fp;

void print_ratio(int p)
{
    fprintf(out_fp, "%.4f\t", ((float) (S1[p]+ST[p])) / (S1[p]+ST[p]+F1[p]+FT[p])); 
}

void print_pred_stats(int p)
{
    fprintf(out_fp,
        "S0: %-4d S1: %-4d SB: %-4d ST: %-4d "
        "F0: %-4d F1: %-4d FB: %-4d FT: %-4d@\n",
         S0[p], S1[p], SB[p], ST[p],
         F0[p], F1[p], FB[p], FT[p]);
}

void print_3_pred(int p, char* func, char* line, char* site, char* u, char* v, char c)
{
    assert(p < NUM_3_PREDS);
    fprintf(out_fp, "%s\t%s\t%s\t%s %c %s@", func, line, site, u, c, v);
    print_pred_stats(p);
}

void print_2_pred(int p, char* func, char* line, char* site, char* u, char c)
{
    assert(p >= NUM_3_PREDS && p < NUM_PREDS);
    fprintf(out_fp, "%s\t%s\t%s\t%s = %c@", func, line, site, u, c);
    print_pred_stats(p);
}

void read_2_line(FILE* fp, char* func, char* line, char* site, char* u)
{
	int c;
        char dummy[500];

        fscanf(fp, "%[^\t]s", dummy);  // read file name
        c = fgetc(fp); assert(c == '\t');
        fscanf(fp, "%[^\t]s", line);   // read line number
        c = fgetc(fp); assert(c == '\t');
        fscanf(fp, "%[^\t]s", func);   // read function name
        c = fgetc(fp); assert(c == '\t');
        fscanf(fp, "%[^\t]s", site);   // read site index
        c = fgetc(fp); assert(c == '\t');
        fscanf(fp, "%[^\n]s", u);      // read operand
        c = fgetc(fp); assert(c == '\n');
}

void read_3_line(FILE* fp, char* func, char* line, char* site, char* u, char* v)
{
	int c;
        char dummy[500];

        fscanf(fp, "%[^\t]s", dummy);    // read file name
        c = fgetc(fp); assert(c == '\t');
        fscanf(fp, "%[^\t]s", line);     // read line number
        c = fgetc(fp); assert(c == '\t');
        fscanf(fp, "%[^\t]s", func);     // read function name
        c = fgetc(fp); assert(c == '\t');
        fscanf(fp, "%[^\t]s", site);     // read site index
        c = fgetc(fp); assert(c == '\t');
        fscanf(fp, "%[^\t]s", u);        // read left operand
        c = fgetc(fp); assert(c == '\t');
        fscanf(fp, "%[^\n]s", v);        // read right operand
        c = fgetc(fp); assert(c == '\n');
}

inline void inc_B(int i, int p)
{
    if (i < NUM_SUCCESS_RUNS)
        SB[p]++;
    else
        FB[p]++;
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
    else
        F1[p]++;
}

inline void inc_T(int i, int p)
{
    if (i < NUM_SUCCESS_RUNS)
        ST[p]++;
    else
        FT[p]++;
}

main()
{
    int i, p, x, y, z, c;
    FILE* fp;

    FILE *fps = fopen(SUCCESS_FILE, "r");
    if (!fps) {
        printf("Error: cannot open file %s\n", SUCCESS_FILE);
        return 1;
    }
    FILE *fpf = fopen(FAILURE_FILE, "r");
    if (!fpf) {
        printf("Error: cannot open file %s\n", FAILURE_FILE);
        return 1;
    }


    for (i = 0; i < NUM_SUCCESS_RUNS; i++)
        fscanf(fps, "%d", &map[i]);
    for (; i < NUM_RUNS; i++)
        fscanf(fpf, "%d", &map[i]);

    for (i = 0; i < NUM_RUNS; i++) {
        char filename[100];
        int pred_count = 0;

        sprintf(filename, PATH_PREFIX "/%d.data3", map[i]);
        fp = fopen(filename, "r");
        if (!fp) {
            printf("Error: cannot read file %s\n", filename);
            return 1;
        }
        for (p = 0; p < NUM_3_SITES; p++) {
            fscanf(fp, "%d %d %d", &x, &y, &z);
            int num_times_exec = x + y + z;
            if (num_times_exec == 0) {
                inc_B(i, pred_count);
                inc_B(i, pred_count + 1);
                inc_B(i, pred_count + 2);
            } else {
                if (!x)
                    inc_0(i, pred_count);
                else if (x == num_times_exec)
                    inc_1(i, pred_count);
                else
                    inc_T(i, pred_count);
                if (!y)
                    inc_0(i, pred_count + 1);
                else if (y == num_times_exec)
                    inc_1(i, pred_count + 1);
                else
                    inc_T(i, pred_count + 1);
                if (!z)
                    inc_0(i, pred_count + 2);
                else if (z == num_times_exec)
                    inc_1(i, pred_count + 2);
                else
                    inc_T(i, pred_count + 2);
            }
            pred_count += 3;
        }
        fclose(fp);

        sprintf(filename, PATH_PREFIX "/%d.data2", map[i]);
        fp = fopen(filename, "r");
        if (!fp) {
            printf("Error: cannot open file %s\n", filename);
            return 1;
        }
        for (p = 0; p < NUM_2_SITES; p++) {
            fscanf(fp, "%d %d", &x, &y);
            int num_times_exec = x + y;
            if (num_times_exec == 0) {
                inc_B(i, pred_count);
                inc_B(i, pred_count + 1);
            } else {
                if (!x)
                    inc_0(i, pred_count);
                else if (x == num_times_exec)
                    inc_1(i, pred_count);
                else
                    inc_T(i, pred_count);
                if (!y)
                    inc_0(i, pred_count + 1);
                else if (y == num_times_exec)
                    inc_1(i, pred_count + 1);
                else
                    inc_T(i, pred_count + 1);
            }
            pred_count += 2;
        }
        fclose(fp);
    }

    /******* PRINT PREDICATE STATISTICS *********/

    out_fp = fopen(STATS_FILE, "w");
    assert(out_fp);

    fp = fopen(SITES_3_FILE, "r");
    assert(fp);
    for (i = 0; i < NUM_3_SITES; i++) {
        char func[300], line[300], site[300], u[500], v[500];
        read_3_line(fp, func, line, site, u, v);
        print_3_pred(i*3,     func, line, site, u, v, '<');
        print_3_pred(i*3 + 1, func, line, site, u, v, '=');
        print_3_pred(i*3 + 2, func, line, site, u, v, '>');
    }
    fclose(fp);

    fp = fopen(SITES_2_FILE, "r");
    assert(fp);
    for (i = 0; i < NUM_2_SITES; i++) {
        char func[300], line[300], site[300], u[500];
        read_2_line(fp, func, line, site, u);
        print_2_pred(NUM_3_PREDS + i*2,     func, line, site, u, 'F');
        print_2_pred(NUM_3_PREDS + i*2 + 1, func, line, site, u, 'T');
    }
    fclose(fp);

    fclose(out_fp);

    /******* COMPUTE CAUSES *********/

    for (p = 0; p < NUM_PREDS; p++)
        causes[p] = 1;

    for (p = 0; p < NUM_PREDS; p++) {
        assert(S0[p] + S1[p] + SB[p] + ST[p] == NUM_SUCCESS_RUNS);
        assert(F0[p] + F1[p] + FB[p] + FT[p] == NUM_FAILURE_RUNS);

/****************************************************************************/
/*************************** EDIT THIS SECTION ******************************/

        // if p is not 0 or T in any passing run, then it is not a smoking gun
        if (S0[p] == 0 && ST[p] == 0)
            causes[p] = 0;
        // if p is not 1 or T in any failing run, then it is not a smoking gun
        if (F1[p] == 0 && FT[p] == 0)
            causes[p] = 0;

/****************************************************************************/
/****************************************************************************/

    }
 
    out_fp = fopen(CAUSES_FILE, "w");
    assert(out_fp);

    fp = fopen(SITES_3_FILE, "r");
    assert(fp);
    for (i = 0; i < NUM_3_SITES; i++) {
        char func[300], line[300], site[300], u[500], v[500];
        read_3_line(fp, func, line, site, u, v);
        if (causes[i*3]) {
            print_ratio (i*3);
            print_3_pred(i*3,     func, line, site, u, v, '<');
        }
        if (causes[i*3 + 1]) {
            print_ratio (i*3 + 1);
            print_3_pred(i*3 + 1, func, line, site, u, v, '=');
        }
        if (causes[i*3 + 2]) {
            print_ratio (i*3 + 2);
            print_3_pred(i*3 + 2, func, line, site, u, v, '>');
        }
    }
    fclose(fp);

    fp = fopen(SITES_2_FILE, "r");
    assert(fp);
    for (i = 0; i < NUM_2_SITES; i++) {
        char func[300], line[300], site[300], u[500];
        read_2_line(fp, func, line, site, u);
        if (causes[NUM_3_PREDS + i*2]) {
            print_ratio (NUM_3_PREDS + i*2);
            print_2_pred(NUM_3_PREDS + i*2,     func, line, site, u, 'F');
        }
        if (causes[NUM_3_PREDS + i*2 + 1]) {
            print_ratio (NUM_3_PREDS + i*2 + 1);
            print_2_pred(NUM_3_PREDS + i*2 + 1, func, line, site, u, 'T');
        }
    }
    fclose(fp);

    fclose(out_fp);

    return 0;
}

