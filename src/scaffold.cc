#include <cstdio>
#include <cassert>
#include "classify_runs.h"
#include "units.h"

void scaffold(const char report_path_fmt[],
              void (*process_s_site)(int r, int u, int c, int x, int y, int z),
              void (*process_r_site)(int r, int u, int c, int x, int y, int z),
              void (*process_b_site)(int r, int u, int c, int x, int y))
{
    int r, u, c;
    int x, y, z;

    for (r = 0; r < num_runs; r++) {
        if (!is_srun[r] && !is_frun[r])
            continue;

        printf("r %d\n", r);
        char s[1000];
        sprintf(s, report_path_fmt, r);

        FILE* fp = fopen(s, "r");
        assert(fp);

        while (1) {
            fscanf(fp, "%d", &u);
            if (feof(fp))
                break;
            switch (units[u].s[0]) {
            case 'S':
                for (c = 0; c < units[u].c; c++) {
                    fscanf(fp, "%d %d %d", &x, &y, &z);
                    assert(x >= 0 && y >= 0 && z >= 0);
                    process_s_site(r, u, c, x, y, z);
                }
                break;
            case 'R':
                for (c = 0; c < units[u].c; c++) {
                    fscanf(fp, "%d %d %d", &x, &y, &z);
                    assert(x >= 0 && y >= 0 && z >= 0);
                    process_r_site(r, u, c, x, y, z);
                }
                break;
            case 'B':
                for (c = 0; c < units[u].c; c++) {
                    fscanf(fp, "%d %d", &x, &y);
                    assert(x >= 0 && y >= 0);
                    process_b_site(r, u, c, x, y);
                }
                break;
            default:
                assert(0);
	    }
	}

	fclose(fp);
    }
}


// Local variables:
// c-file-style: "cc-mode"
// End:
