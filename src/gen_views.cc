#include <cassert>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <libgen.h>
#include <string>
#include <sys/stat.h>
#include "def.h"
#include "shell.h"
#include "utils.h"

#define CBI_WEBPAGE "http://www.cs.berkeley.edu/~liblit/sampler/"

char* result_summary_file = NULL;
char* prefix = NULL;
std::string program_src_dir;

void process_cmdline(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
	if (!strcmp(argv[i], "-r")) {
	    i++;
	    result_summary_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-p")) {
	    i++;
	    prefix = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-d")) {
	    i++;
	    program_src_dir = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-verbose")) {
	    verbose = true;
	    continue;
	}
	if (!strcmp(argv[i], "-h")) {
	    puts("Usage: gen-views -r <result_summary_file> -p <prefix> -d <srcdir> [-verbose]");
	    exit(0);
	}
	printf("Illegal option: %s\n", argv[i]);
	exit(1);
    }

    if (!result_summary_file || !prefix || program_src_dir.empty()) {
	puts("Incorrect usage; try -h");
	exit(1);
    }
}

int main(int argc, char** argv)
{
    int i, m;
    FILE* fp;

    process_cmdline(argc, argv);

    bool exists[NUM_SCHEMES];
    for (i = 0; i < NUM_SCHEMES; i++) {
	char unsorted[NAME_MAX];
	snprintf(unsorted, sizeof(unsorted), "%s.txt", scheme_codes[i]);

	struct stat stats;
	exists[i] = stat(unsorted, &stats) == 0 && stats.st_size;
    }

    for (i = 0; i < NUM_SCHEMES; i++)
	if (exists[i])
	    for ( m = 0; m < NUM_SORTBYS; m++) {

		char file[1000];
		sprintf(file, "%s_%s.%s.xml", scheme_codes[i], sortby_codes[m], prefix);
		fp = fopen(file, "w");
		assert(fp);

		fprintf(fp,
			"<?xml version=\"1.0\"?>"
			"<?xml-stylesheet type=\"text/xsl\" href=\"view.xsl\"?>"
			"<!DOCTYPE view SYSTEM \"view.dtd\">"
			"<view prefix=\"%s\" summary=\"%s\">"
			"<schemes current=\"%s\">",
			prefix, result_summary_file,
			scheme_codes[i]);

		for (int j = 0; j < NUM_SCHEMES; j++)
		    if (exists[j])
			fprintf(fp, "<scheme code=\"%s\" name=\"%s\"/>", scheme_codes[j], scheme_names[j]);

		fprintf(fp, "</schemes><sorts current=\"%s\"/><predictors>", sortby_codes[m]);

		char file2[100];
		sprintf(file2, "%s.tmp.txt", scheme_codes[i]);

		shell("sort -k%d -r -n < %s.txt > %s", sortby_indices[m], scheme_codes[i], file2);

		FILE* fp2 = fopen(file2, "r");
		assert(fp2);

		while (1) {
		    pred_info pi;
		    const bool got = read_pred_full(fp2, pi);
		    if (!got)
			break;
		    print_pred_full(fp, program_src_dir, pi);
		}

		fclose(fp2);

		fputs("</predictors></view>\n", fp);

		fclose(fp);
	    }

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
