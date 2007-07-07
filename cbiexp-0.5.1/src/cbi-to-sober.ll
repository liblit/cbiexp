%{ // -*- c++ -*-
#include <fstream>
#include <iostream>
#include <sstream>
#include "RawReport.h"
#include "fopen.h"

using namespace std;


%}


%option 8bit
%option align
%option fast
%option nodefault
%option nounput
%option noyywrap
%option read

%x REPORT
%x REPORT_SKIP
%x SITES_2
%x SITES_3
%x SITES_SKIP


uint 0|[1-9][0-9]*


%%


{
    BEGIN(INITIAL);
}


<INITIAL><report\ id=\"samples\">\n {
    BEGIN(REPORT);
}

<INITIAL><report\ id=\".*\">\n {
    BEGIN(REPORT_SKIP);
}

<REPORT,REPORT_SKIP><\/report>\n {
    BEGIN(INITIAL);
}

<REPORT_SKIP>.*\n {
}


<REPORT><samples\ unit=\"[0-9a-f]{32}\"\ scheme=\"branches\">\n {
    BEGIN(SITES_2);
}

<REPORT><samples\ unit=\"[0-9a-f]{32}\"\ scheme=\"returns\">\n {
    BEGIN(SITES_3);
}

<REPORT><samples\ unit=\"[0-9a-f]{32}\"\ scheme=\"[-a-z]+\">\n {
    BEGIN(SITES_SKIP);
}


<SITES_2>0\t0\n {
    cout << "0 0 0 0 0 0 ";
}

<SITES_2>{uint}\t{uint}\n {
    istringstream parse(yytext);
    unsigned falseCount, trueCount;
    parse >> falseCount >> trueCount;
    const unsigned sum = falseCount + trueCount;
    cout << falseCount << ' ' << trueCount << ' ' << sum << ' '
	 << trueCount << ' ' << falseCount << ' ' << sum << ' ';
}


<SITES_3>0\t0\t0\n {
    cout << "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ";
}

<SITES_3>{uint}\t{uint}\t{uint}\n {
    istringstream parse(yytext);
    unsigned lt, eq, gt;
    parse >> lt >> eq >> gt;
    const unsigned sum = lt + eq + gt;
    cout << lt << ' ' << sum - lt << ' ' << sum << ' '
	 << sum - lt << ' ' << lt << ' ' << sum << ' '
	 << eq << ' ' << sum - eq << ' ' << sum << ' '
	 << sum - eq << ' ' << eq << ' ' << sum << ' '
	 << gt << ' ' << sum - gt << ' ' << sum << ' '
	 << sum - gt << ' ' << gt << ' ' << sum << ' ';
}


<SITES_2,SITES_3,SITES_SKIP><\/samples>\n {
    BEGIN(REPORT);
}

<SITES_SKIP>.*\n {
}


<INITIAL>.*\n {
}

<*>.*\n? {
    cerr << "reports file corrupted near \"" << yytext << "\"\n";
    return true;
}


<INITIAL><<EOF>> {
    cout << '\n';
    return false;
}


%%


#include <argp.h>
#include "ClassifyRuns.h"
#include "NumRuns.h"
#include "RunsDirectory.h"
#include "StaticSiteInfo.h"

using namespace std;


static char *
processCommandLine(int argc, char *argv[])
{
  static const argp_child children[] = {
    { &NumRuns::argp, 0, 0, 0 },
    { &RunsDirectory::argp, 0, 0, 0 },
    { 0, 0, 0, 0 }
  };

  static const argp argp = {
    0, 0, 0, 0, children, 0, 0
  };

  int remainder;
  argp_parse(&argp, argc, argv, 0, &remainder, 0);
  return argv[remainder];
}


int main(int argc, char *argv[])
{
  set_terminate(__gnu_cxx::__verbose_terminate_handler);
  processCommandLine(argc, argv);
  ios::sync_with_stdio(false);

  bool failed = false;
  unsigned runId;
  cin.exceptions(ios::badbit);

  while (cin >> runId && runId < NumRuns::end)
    if (runId >= NumRuns::begin)
      {
	const string infile(RawReport::format(runId));
	FILE* ifp = fopenRead(infile);
	yyrestart(ifp);
	failed |= yylex();
	fclose(ifp);
      }

  return failed;
}
