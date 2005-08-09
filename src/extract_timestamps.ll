%{ // -*- c++ -*-
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include "Units.h"

using namespace std;



static FILE *ofp1 = NULL;
static FILE *ofp2 = NULL;

static int get_unit_indx(char scheme_code, const string &signature);

static Units units;
 
static string signature;
static string scheme;
static string when;

static unsigned unitIndex;
static unsigned sitesActual;
static unsigned sitesExpected;

#define YY_DECL int yylex(const string &infile)
 
%}


%option 8bit
%option align
%option fast
%option nodefault
%option nounput
%option read

%x SITES_F
%x SITES_L


uint 0|[1-9][0-9]*


%%


	{
	    /* Explicit reset in case previous file ended abruptly. */
	    BEGIN(INITIAL);
	}


<INITIAL><timestamps\ unit=\"[0-9a-f]{32}\"\ scheme=\"[-a-z]+\"\ when=\"[a-z]+\">\n {
    /* Initially look for a starting <timestamps> tag. */

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

	t++;
	const char * const whenPtr = strchr(t, '\"') + 1;
	t = strchr(whenPtr, '\"');
	*t = '\0';
	when = whenPtr;
    }

    char whenCode;
    if (when == "first") {
        whenCode = 'F';
	BEGIN(SITES_F);
    } else if (when == "last") {
	whenCode = 'L';
	BEGIN(SITES_L);
    } else {
	cerr << "unknown timestamp method \"" << when << "\"\n";
        exit(1);
    }

    char schemeCode;
    if (scheme == "scalar-pairs") {
	schemeCode = 'S';
    } else if (scheme == "branches") {
	schemeCode = 'B';
    } else if (scheme == "returns") {
	schemeCode = 'R';
    } else if (scheme == "g-object-unref") {
	schemeCode = 'G';
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


<SITES_F,SITES_L><\/timestamps>\n {
    /* Counts end at the closing </timestamps> tag. */
    /* Sanity check that we got as many sites as we expected. */
    if (sitesActual != sitesExpected) {
	cerr << "site count mismatch: got " << sitesActual << ", expected " << sitesExpected << '\n'
	     << "\tunit: " << signature << '\n'
	     << "\tscheme: " << scheme << '\n'
	     << "\twhen: " << when << '\n';
	exit(1);
    }
    BEGIN(INITIAL);
}

<SITES_F>0\n {
    ++sitesActual;
}

<SITES_F>{uint}\n {
    fprintf(ofp1, "%u\t%u\t%s", unitIndex, sitesActual, yytext);
    ++sitesActual;
}

<SITES_L>0\n {
    ++sitesActual;
}

<SITES_L>{uint}\n {
    fprintf(ofp2, "%u\t%u\t%s", unitIndex, sitesActual, yytext);
    ++sitesActual;
}

<SITES_F,SITES_L>.*\n|. {
    cerr << "\nmalformed input near \"" << yytext << "\"\n"
	 << "\tinput: " << infile << '\n'
	 << "\tunit: " << signature << '\n'
	 << "\tscheme: " << scheme << '\n'
	 << "\twhen: " << when << '\n'
	 << "\tsite: " << sitesActual << '\n';
    return 1;
}


%%


#include <argp.h>
#include "TimestampReport.h"
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
    Progress::Bounded progress("extracting timestamps", NumRuns::count());

    for (unsigned i = NumRuns::begin; i < NumRuns::end; i++) {
	progress.step();
	if (!is_srun[i] && !is_frun[i])
	    continue;

	const string infile(RawReport::format(i));
	FILE* ifp = fopenRead(infile);
	ofp1 = fopenWrite(TimestampReport::format(i, "first"));
	ofp2 = fopenWrite(TimestampReport::format(i, "last"));

	yyrestart(ifp);
	failed |= yylex(infile);

	fclose(ifp);
	fclose(ofp1);
	fclose(ofp2);
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
