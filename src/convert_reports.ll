%{ // -*- c++ -*-
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include "Units.h"

using namespace std;


static FILE *ofp = NULL;

static int get_unit_indx(char scheme_code, const string &signature);

static string signature;
static string scheme;

static unsigned unitIndex;
static unsigned sitesActual;
static unsigned sitesExpected;

static Units units;

#define YY_DECL int yylex(const string &infile)
 
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

    unitIndex = get_unit_indx(schemeCode, signature);
    assert(units[unitIndex].scheme_code == schemeCode);
    sitesExpected = units[unitIndex].num_sites;
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

<SITES_2>0\t0\n {
    ++sitesActual;
}

<SITES_2>{uint}\t{uint}\n {
    fprintf(ofp, "%u\t%u\t%s", unitIndex, sitesActual, yytext);
    ++sitesActual;
}

<SITES_3>0\t0\t0\n {
    ++sitesActual;
}

<SITES_3>{uint}\t{uint}\t{uint}\n {
    fprintf(ofp, "%u\t%u\t%s", unitIndex, sitesActual, yytext);
    ++sitesActual;
}

<SITES_4>0\t0\t0\t0\n {
    ++sitesActual;
}

<SITES_4>{uint}\t{uint}\t{uint}\t{uint}\n {
    fprintf(ofp, "%u\t%u\t%s", unitIndex, sitesActual, yytext);
    ++sitesActual;
}


<SITES_2,SITES_3,SITES_4>.*\n|. {
    cerr << "\nmalformed input near \"" << yytext << "\"\n"
	 << "\tinput: " << infile << '\n'
	 << "\tunit: " << signature << '\n'
	 << "\tscheme: " << scheme << '\n'
	 << "\tsite: " << sitesActual << '\n';
    return 1;
}


%%


#include <argp.h>
#include "CompactReport.h"
#include "NumRuns.h"
#include "Progress/Bounded.h"
#include "RawReport.h"
#include "RunsDirectory.h"
#include "classify_runs.h"
#include "fopen.h"
#include "utils.h"

static int get_unit_indx(char scheme_code, const string &signature)
{
    for (unsigned i = 0; i < units.size; i++)
	if (scheme_code == units[i].scheme_code && signature == units[i].signature)
	    return i;

    cerr << "cannot find unit index\n"
	 << "\tunit: " << signature << '\n'
	 << "\tscheme: " << scheme_code << '\n';
    exit(1);
}

void process_cmdline(int argc, char** argv)
{
    static const argp_child children[] = {
	{ &NumRuns::argp, 0, 0, 0 },
	{ &RunsDirectory::argp, 0, 0, 0 },
	{ 0, 0, 0, 0 }
    };

    static const argp argp = {
	0, 0, 0, 0, children, 0, 0
    };

    argp_parse(&argp, argc, argv, 0, 0, 0);
}

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);

    classify_runs();

    bool failed = false;
    Progress::Bounded progress("converting reports", NumRuns::count());

    for (unsigned i = NumRuns::begin; i < NumRuns::end; i++) {
	progress.step();
	if (!is_srun[i] && !is_frun[i])
	    continue;

	const string infile(RawReport::format(i));
	FILE* ifp = fopenRead(infile);
	ofp = fopenWrite(CompactReport::format(i));

	yyrestart(ifp);
	failed |= yylex(infile);

	fclose(ifp);
	fclose(ofp);
    }

    return failed;
}


/*
	system("rm -f ./reports");
	sprintf(cmd, "gunzip -c /moa/sc2/cbi/rhythmbox/data/%d/client/reports.gz > reports", i);
	system(cmd);
*/


// Local variables:
// c-file-style: "cc-mode"
// End:
