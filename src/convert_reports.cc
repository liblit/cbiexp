#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include "classify_runs.h"
#include "units.h"

char* sruns_txt_file = NULL;
char* fruns_txt_file = NULL;
char* verbose_report_path_fmt = NULL;
char* compact_report_path_fmt = NULL;

char get_scheme_code(char* scheme_str)
{
    if (!strcmp(scheme_str, "scalar-pairs"))   return 'S';
    if (!strcmp(scheme_str, "branches"    ))   return 'B';
    if (!strcmp(scheme_str, "returns"     ))   return 'R';
    if (!strcmp(scheme_str, "g-object-unref")) return 'G';
    assert(0);
}

int get_unit_indx(char scheme_code, const char* signature)
{
    for (int i = 0; i < num_units; i++)
        if (scheme_code == units[i].scheme_code && strcmp(signature, units[i].signature) == 0)
            return i;
    assert(0);
}

void process_cmdline(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
	if (!strcmp(argv[i], "-vr")) {
	    i++;
	    verbose_report_path_fmt = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-cr")) {
	    i++;
	    compact_report_path_fmt = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-s")) {
	    i++;
	    sruns_txt_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-f")) {
	    i++;
	    fruns_txt_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-h")) {
	    puts("Usage: convert-reports <options>\n"
                 "(r) -s  <sruns-file>\n"
                 "(r) -f  <fruns-file>\n"
                 "(r) -vr <verbose-report-path-fmt>\n"
                 "(w) -cr <compact-report-path-fmt>\n"
            );
	    exit(0);
	}
	printf("Illegal option: %s\n", argv[i]);
	exit(1);
    }

    if (!sruns_txt_file || !fruns_txt_file || !verbose_report_path_fmt || !compact_report_path_fmt) {
	puts("Incorrect usage; try -h");
	exit(1);
    }
}

int main(int argc, char** argv)
{
    char s[3000], *signature, *scheme_str, *t;
    char p[3000];

    process_cmdline(argc, argv);

    classify_runs(sruns_txt_file, fruns_txt_file);

    for (int i = 0; i < num_runs; i++) {
	char ifile[1000], ofile[1000];

	if (!is_srun[i] && !is_frun[i])
	    continue;

	sprintf(ifile, verbose_report_path_fmt, i);
	FILE* ifp = fopen(ifile, "r");
	assert(ifp);

	sprintf(ofile, compact_report_path_fmt, i);
	FILE* ofp = fopen(ofile, "w");
	assert(ofp);

	printf("r %d\n", i);

	fgets(s, 3000, ifp);
	assert(strncmp(s, "<rep", 4) == 0);

	while (1) {
	    fgets(s, 3000, ifp);
	    if (feof(ifp))
		break;
	    if (strncmp(s, "<sam", 4) != 0)
		break;
	    signature = strchr(s, '\"');
	    signature++;
	    t = strchr(signature, '\"');
	    *t = '\0';
	    t++;
	    scheme_str = strchr(t, '\"');
	    scheme_str++;
	    t = strchr(scheme_str, '\"');
	    *t = '\0';

            int indx = get_unit_indx(get_scheme_code(scheme_str), signature);
	    fprintf(ofp, "%d\n", indx);

	    int count = 0;
	    while (1) {
		fgets(p, 3000, ifp);
		if (strncmp(p, "</sam", 5) == 0)
		    break;
		fputs(p, ofp);
		count++;
	    }
	    assert(count == units[indx].num_sites);
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


// Local variables:
// c-file-style: "cc-mode"
// End:
