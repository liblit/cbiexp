#include <cstdio>
#include <cassert>
#include "classify_runs.h"

void scaffold(char* report_path_fmt, void (*process_site)(FILE*, int, int, int))
{
    int r, u, c;

    for (r = 0; r < num_runs; r++) {
	if (!is_srun[r] && !is_frun[r])
	    continue;
	printf("r %d\n", r);
	char s[1000];
	sprintf(s, report_path_fmt, r);
	FILE* fp = fopen(s, "r");
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
