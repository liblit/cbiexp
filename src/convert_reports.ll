%{ // -*- c++ -*-
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include "units.h"

using namespace std;


static FILE *ofp = NULL;

static int get_unit_indx(char scheme_code, const string &signature);

string signature;
string scheme;
unsigned sitesActual;
unsigned sitesExpected;

%}


%option 8bit
%option align
%option fast
%option nodefault
%option nounput
%option read

%x SITES_2
%x SITES_3
%x SITES_4


uint 0|[1-9][0-9]*


%%


	{
	    /* Explicit reset in case previous file ended abruptly. */
	    BEGIN(INITIAL);
	}


<INITIAL><samples\ unit=\"[0-9a-f]{32}\"\ scheme=\"[-a-z]+\">\n {
    /* Initially look for a starting <samples> tag. */

    {
	const char * const signaturePtr = strchr(yytext, '\"') + 1;
	char *t = strchr(signaturePtr, '\"');
	*t = '\0';
	signature = signaturePtr;

	t++;
	const char * const schemePtr = strchr(t, '\"') + 1;
	t = strchr(schemePtr, '\"');
	*t = '\0';
	scheme = schemePtr;
    }

    char schemeCode;
    if (scheme == "scalar-pairs") {
	schemeCode = 'S';
	BEGIN(SITES_3);
    } else if (scheme == "branches") {
	schemeCode = 'B';
	BEGIN(SITES_2);
    } else if (scheme == "returns") {
	schemeCode = 'R';
	BEGIN(SITES_3);
    } else if (scheme == "g-object-unref") {
	schemeCode = 'G';
	BEGIN(SITES_4);
    } else {
	cerr << "unknown scheme \"" << scheme << "\"\n";
	exit(1);
    }

    const int indx = get_unit_indx(schemeCode, signature);
    assert(units[indx].scheme_code == schemeCode);
    fprintf(ofp, "%d\n", indx);
    sitesExpected = units[indx].num_sites;
    sitesActual = 0;
}

<INITIAL>[^<]+|< {
    /* Silently discard any other junk. */
}


<SITES_2,SITES_3,SITES_4><\/samples>\n {
    /* Counts end at the closing </samples> tag. */
    /* Sanity check that we got as many sites as we expected. */
    if (sitesActual != sitesExpected) {
	cerr << "site count mismatch: got " << sitesActual << ", expected " << sitesExpected << '\n'
	     << "\tunit: " << signature << '\n'
	     << "\tscheme: " << scheme << '\n';
	exit(1);
    }
    BEGIN(INITIAL);
}

<SITES_2>{uint}\t{uint}\n {
    ++sitesActual;
    fputs(yytext, ofp);
}

<SITES_3>{uint}\t{uint}\t{uint}\n {
    ++sitesActual;
    fputs(yytext, ofp);
}

<SITES_4>{uint}\t{uint}\t{uint}\t{uint}\n {
    ++sitesActual;
    fputs(yytext, ofp);
}


<SITES_2,SITES_3,SITES_4>.*\n|. {
    cerr << "malformed input near \"" << yytext << "\"\n"
	 << "\tunit: " << signature << '\n'
	 << "\tscheme: " << scheme << '\n'
	 << "\tsite: " << sitesActual << '\n';
    exit(1);
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

static int get_unit_indx(char scheme_code, const string &signature)
{
    for (int i = 0; i < num_units; i++)
	if (scheme_code == units[i].scheme_code && signature == units[i].signature)
	    return i;

    cerr << "cannot find unit index\n"
	 << "\tunit: " << signature << '\n'
	 << "\tscheme: " << scheme_code << '\n';
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
	if (!ifp) {
	    fprintf(stderr, "cannot read %s: %s\n", ifile, strerror(errno));
	    exit(1);
	}

	sprintf(ofile, compact_report_path_fmt, i);
	ofp = fopen(ofile, "w");
	if (!ofp) {
	    fprintf(stderr, "cannot write %s: %s\n", ofile, strerror(errno));
	    exit(1);
	}

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
