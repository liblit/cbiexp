#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define NUM_RUNS    31996
#define NUM_R_SITES   494
#define NUM_B_SITES  2085
#define NUM_S_SITES 32644

bool is_srun[NUM_RUNS];
bool is_frun[NUM_RUNS];
int run_order[NUM_RUNS];

int num_R = 0, num_B = 0, num_S = 0;
int ds = -1;
int nr = 0;
int chunksize = 3000;

struct site_t {
    int S[6], F[6];
    int os, of;
    site_t() 
    { 
        os = of = 0;
        for (int i = 0; i < 6; i++) {
            S[i] = 0;
            F[i] = 0;
        }
    }
};

site_t R[NUM_R_SITES];
site_t B[NUM_B_SITES];
site_t S[NUM_S_SITES];

#define incR(r, s, p)      \
{                          \
    if (is_srun[r])        \
        R[s].S[p]++;       \
    else if (is_frun[r])   \
        R[s].F[p]++;       \
}

#define incB(r, s, p)      \
{                          \
    if (is_srun[r])        \
        B[s].S[p]++;       \
    else if (is_frun[r])   \
        B[s].F[p]++;       \
}

#define incS(r, s, p)      \
{                          \
    if (is_srun[r])        \
        S[s].S[p]++;       \
    else if (is_frun[r])   \
        S[s].F[p]++;       \
}

#define obsR(r, s)         \
{                          \
    if (is_srun[r])        \
        R[s].os++;         \
    else if (is_frun[r])   \
        R[s].of++;         \
}

#define obsB(r, s)         \
{                          \
    if (is_srun[r])        \
        B[s].os++;         \
    else if (is_frun[r])   \
        B[s].of++;         \
}

#define obsS(r, s)         \
{                          \
    if (is_srun[r])        \
        S[s].os++;         \
    else if (is_frun[r])   \
        S[s].of++;         \
}

void count_R(int site, int p)
{
    int s  = R[site].S[p];
    int f  = R[site].F[p];
    int os = R[site].os;
    int of = R[site].of;

    float cr = ((float)  f) / ( s +  f);
    float co = ((float) of) / (os + of);
    float in = cr - co;

    if (in - 1.96 * sqrt(((cr * (1 - cr)) / (f + s)) + ((co * (1 - co))/(of + os))) > 0 && s + f >= 10)
        num_R++;
}

void count_B(int site, int p)
{
    int s  = B[site].S[p];
    int f  = B[site].F[p];
    int os = B[site].os;
    int of = B[site].of;

    float cr = ((float)  f) / ( s +  f);
    float co = ((float) of) / (os + of);
    float in = cr - co;

    if (in - 1.96 * sqrt(((cr * (1 - cr)) / (f + s)) + ((co * (1 - co))/(of + os))) > 0 && s + f >= 10)
        num_B++;
}

void count_S(int site, int p)
{
    int s  = S[site].S[p];
    int f  = S[site].F[p];
    int os = S[site].os;
    int of = S[site].of;

    float cr = ((float)  f) / ( s +  f);
    float co = ((float) of) / (os + of);
    float in = cr - co;

    if (in - 1.96 * sqrt(((cr * (1 - cr)) / (f + s)) + ((co * (1 - co))/(of + os))) > 0 && s + f >= 10)
        num_S++;
}

int main(int argc, char** argv)
{
    int i, j, x, y, z;
    FILE *sfp = NULL, *ffp = NULL, *pfp = NULL;
    char s[3000];

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
	if (strcmp(argv[i], "-p") == 0) {
            if (pfp) {
                printf("-p specified multiple times\n");
                return 1;
            }
            i++;
            if (!argv[i]) {
                printf("argument to -p missing\n");
                return 1;
            }
            pfp = fopen(argv[i], "r");
            if (!pfp) {
                printf("Cannot open file %s for reading\n", argv[i]);
                return 1;
            }
            continue;
	}
        if (strcmp(argv[i], "-d") == 0) {
            if (ds != -1) {
                printf("-d specified multiple times\n");
                return 1;
            }
            i++;
            if (!argv[i]) {
                printf("argument to -d missing\n");
                return 1;
            }
            ds = atoi(argv[i]);
            if (ds != 1 && ds != 10 && ds != 100 && ds != 1000) {
                printf("argument to -d must be 1 or 10 or 100 or 1000\n");
                return 1;
            }
            continue;
        }
        if (strcmp(argv[i], "-h") == 0) {
            printf("Usage: compute -s runs_file -f runs_file -p order -d 1|10|100|1000\n");
            return 0;
        }
        printf("Illegal option: %s\n", argv[i]);
        return 1;
    }

    if (!sfp || !ffp || !pfp) {
        printf("Incorrect usage; try -h\n");
        return 1;
    }

    /************************************************************************
     ** read -s specified file 
     ************************************************************************/

    memset(is_srun, 0, NUM_RUNS*sizeof(bool));
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

    memset(is_frun, 0, NUM_RUNS*sizeof(bool));
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

    for (i = 0; i < NUM_RUNS; i++) {
        if (is_srun[i] && is_frun[i]) {
            printf("Run %d is both successful and failing\n", i);
            return 1;
        }
    }

    /************************************************************************
     ** read -p specified file 
     ************************************************************************/

    nr = 0;
    memset(run_order, 0, NUM_RUNS*sizeof(int));
    while (1) {
        fscanf(pfp, "%d", &i);
        if (feof(pfp))
            break;
        if (i < 0 || i >= NUM_RUNS) {
            printf("Illegal run %d in -f specified file\n", i);
            return 1;
        }
        run_order[nr++] = i;
    }
    fclose(pfp);
    printf("Using %d runs total.\n", nr);



    /************************************************************************
     ** compute for each predicate, number of successful and failing runs in
     ** which it is (1) just observed, and (2) observed to be true.
     ** proceed in chunks of runs of size <chunksize>
     ** output incremental results
     ************************************************************************/


    int ns = 0, nf = 0, start = 0, end = 0;
    for (int ctr = 0; ctr < (int) ceil((double) nr/chunksize); ctr++) {
	num_R = 0; num_S = 0; num_B = 0;
	end += chunksize;
	if (end > nr)
		end = nr;

    	for (int r = start; r < end; r++) {
		i = run_order[r];
        	if (is_srun[i])
            	ns++;
        	else if (is_frun[i])
            	nf++;
        	else if (!is_srun[i] && !is_frun[i])
            	continue;


        	char file[100];

        	switch (ds) {
        	case 1   : sprintf(file, "/moa/sc1/ds1/%d.txt"   , i); break;
        	case 10  : sprintf(file, "/moa/sc2/ds10/%d.txt"  , i); break;
        	case 100 : sprintf(file, "/moa/sc3/ds100/%d.txt" , i); break;
        	case 1000: sprintf(file, "/moa/sc4/ds1000/%d.txt", i); break;
        	}

        	FILE* fp = fopen(file, "r");
        	assert(fp);

        	fgets(s, 1000, fp);  // eat <report id ...
        	//printf("**0 %s", s);
        	if (ds != 1) {
            	fgets(s, 1000, fp);  // eat <samples R ...
            	//printf("**0 %s", s);
            	assert(strncmp(s, "<samp", 5) == 0);
        	}

        	for (j = 0; j < NUM_R_SITES; j++) {
            	fscanf(fp, "%d %d %d\n", &x, &y, &z); 
            	//printf("%d %d %d\n", x, y, z);
            	if (x + y + z > 0) {
                	obsR(i, j);
                	if (x > 0) {
                    	incR(i, j, 0);
                    	incR(i, j, 3);
                    	incR(i, j, 5);
                	}
                	if (y > 0) {
                    	incR(i, j, 2);
                    	incR(i, j, 1);
                    	incR(i, j, 5);
                	}
                	if (z > 0) {
                    	incR(i, j, 4);
                    	incR(i, j, 1);
                    	incR(i, j, 3);
                	}
            	}
        	}

        	fgets(s, 1000, fp);  // eat </samples ...
        	//printf("**1 %s", s);
        	if (ds != 1) {
            	fgets(s, 1000, fp);    // eat <samples B ...
            	//printf("**1 %s", s);
            	assert(strncmp(s, "<samp", 5) == 0);
        	}

        	for (j = 0; j < NUM_B_SITES; j++) {
            	fscanf(fp, "%d %d\n", &x, &y); 
            	//printf("%d %d\n", x, y);
            	if (x + y > 0) {
                	obsB(i, j);
                	if (x > 0) incB(i, j, 0);
                	if (y > 0) incB(i, j, 1);
            	} 
        	}

        	fgets(s, 1000, fp);  // eat </samples ...
        	//printf("**2 %s", s);
        	if (ds != 1) {
            	fgets(s, 1000, fp);  // eat <samples S ...
            	//printf("**2 %s", s);
            	assert(strncmp(s, "<samp", 5) == 0);
        	}

        	for (j = 0; j < NUM_S_SITES; j++) {
            	fscanf(fp, "%d %d %d\n", &x, &y, &z); 
            	//printf("%d %d %d\n", x, y, z);
            	if (x + y + z > 0) {
                	obsS(i, j);
                	if (x > 0) {
                    	incS(i, j, 0);
                    	incS(i, j, 3);
                    	incS(i, j, 5);
                	}
                	if (y > 0) {
                    	incS(i, j, 2);
                    	incS(i, j, 1);
                    	incS(i, j, 5);
                	}
                	if (z > 0) {
                    	incS(i, j, 4);
                    	incS(i, j, 1);
                    	incS(i, j, 3);
                	}
            	}
        	}
        	fclose(fp);
    	}

    	for (j = 0; j < NUM_R_SITES; j++) {
        	count_R(j, 0);
        	count_R(j, 1);
        	count_R(j, 2);
        	count_R(j, 3);
        	count_R(j, 4);
        	count_R(j, 5);
    	}
    	for (j = 0; j < NUM_B_SITES; j++) {
        	count_B(j, 0);
        	count_B(j, 1);
    	}
    	for (j = 0; j < NUM_S_SITES; j++) {
        	count_S(j, 0);
        	count_S(j, 1);
        	count_S(j, 2);
        	count_S(j, 3);
        	count_S(j, 4);
        	count_S(j, 5);
    	}

    	printf("Ctr %d: #S runs: %d #F runs: %d\n", ctr, ns, nf);
    	printf("count of B preds: %d\n", num_B);
    	printf("count of R preds: %d\n", num_R);
    	printf("count of S preds: %d\n", num_S);

	start += chunksize;
	}
    	return 0;
}

