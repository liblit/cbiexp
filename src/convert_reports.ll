%{ // -*- c++ -*-
#include <cassert>
#include <cstdio>
#include <cstring>
#include "units.h"


static FILE *ofp = NULL;

static char get_scheme_code(char* scheme_str);
static int get_unit_indx(char scheme_code, const char* signature);
 
%}


%option 8bit
%option align
%option fast
%option nodefault
%option nounput
%option read

%x COUNTS


%%


	{
	    /* Explicit reset in case previous file ended abruptly. */
	    BEGIN(INITIAL);
	}


<INITIAL><samples\ unit=\"[0-9a-f]{32}\"\ scheme=\"[-a-z]+\">\n {
    /* Initially look for a starting <samples> tag. */
    BEGIN(COUNTS);

    const char *signature = strchr(yytext, '\"');
    signature++;
    char *t = strchr(signature, '\"');
    *t = '\0';
    t++;
    char *scheme_str = strchr(t, '\"');
    scheme_str++;
    t = strchr(scheme_str, '\"');
    *t = '\0';

    const int indx = get_unit_indx(get_scheme_code(scheme_str), signature);
    fprintf(ofp, "%d\n", indx);
}

<INITIAL>[^<]+|< {
    /* Silently discard any other junk. */
}


<COUNTS><\/samples> {
    /* Counts end at the closing </samples> tag. */
    /* Sanity check that we got as many sites as we expected. */
    BEGIN(INITIAL);
}

<COUNTS>[^<]+|< {
    /* Copy counts to ouput unchanged. */
    fputs(yytext, ofp);
}


%%


#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "classify_runs.h"

char* sruns_txt_file = NULL;
char* fruns_txt_file = NULL;
char* verbose_report_path_fmt = NULL;
char* compact_report_path_fmt = NULL;

static char get_scheme_code(char* scheme_str)
{
    if (!strcmp(scheme_str, "scalar-pairs"))   return 'S';
    if (!strcmp(scheme_str, "branches"    ))   return 'B';
    if (!strcmp(scheme_str, "returns"     ))   return 'R';
    if (!strcmp(scheme_str, "g-object-unref")) return 'G';
    assert(0);
}

static int get_unit_indx(char scheme_code, const char* signature)
{
    for (int i = 0; i < num_units; i++)
        if (scheme_code == units[i].scheme_code && strcmp(signature, units[i].signature) == 0)
            return i;

    fprintf(stderr, "no static site information for dynamic report:\n\tsignature: %s\n\tscheme: %c\n", signature, scheme_code);
    exit(1);
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
	ofp = fopen(ofile, "w");
	assert(ofp);

	printf("r %d\n", i);
	yyrestart(ifp);
	yylex();

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
