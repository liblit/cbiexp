#include <stdio.h>
#include <assert.h>
#include "def.h"
#include UNITS_HDR_FILE
#include "classify_runs.h"

void scaffold(void (*process_site)(FILE*, int, int, int))
{
    int r, u, c;

    for (r = 0; r < num_runs; r++) {
        if (!is_srun[r] && !is_frun[r])
            continue;
        printf("r %d\n", r);
        char file[1000];
        sprintf(file, O_REPORT_PATH_FMT, r);
        FILE* fp = fopen(file, "r");
        assert(fp);

        while (1) {
            fscanf(fp, "%d\n", &u);
            if (feof(fp))
                break;
            for (c = 0; c < units[u].c; c++)
                process_site(fp, r, u, c);
        }
        fclose(fp);
    }
}

