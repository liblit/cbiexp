/*-*- c++ -*-*/

%{
#include <iostream>
#include <ext/hash_map>
#include <gsl/gsl_randist.h>
#include "SiteCoords.h"

static gsl_rng *generator;

 static double probability;
static unsigned unitIndex;

typedef __gnu_cxx::hash_map<SiteCoords, double> Plan;
static Plan plan;
%}


%option nounput
%option noyywrap

%x SITE
%x COUNTS
%x EXEMPT


%%


<INITIAL>^[0-9]+\t	{
  fputs(yytext, stdout);
  unitIndex = strtoul(yytext, 0, 10);
  BEGIN(SITE);
}

<SITE>[0-9]+\t	{
  fputs(yytext, stdout);
  const unsigned siteOffset = strtoul(yytext, 0, 10);
  const SiteCoords coords(unitIndex, siteOffset);
  const Plan::const_iterator found = plan.find(coords);
  if (found == plan.end() || found->second == 1)
    BEGIN(EXEMPT);
  else
    {
      probability = found->second;
      BEGIN(COUNTS);
    }
}

<COUNTS>[1-9][0-9]*	{
  /* above pattern matches nonzero counts only */
  /* zero counts pass through unchanged */
  const unsigned complete = strtoul(yytext, 0, 10);
  const unsigned reduced = gsl_ran_binomial(generator, probability, complete);
  printf("%u", reduced);
}

<COUNTS,EXEMPT>\n	{
  fputc('\n', stdout);
  BEGIN(INITIAL);
}


%%


#include <argp.h>
#include <fstream>
#include <sysexits.h>
#include "CompactReport.h"

using namespace std;


////////////////////////////////////////////////////////////////////////
//
//  command line parsing
//


static istream &
operator>>(istream &in, pair<SiteCoords, double> &entry)
{
  return in >> entry.first >> entry.second;
}


static int
parseFlag(int key, char *arg, argp_state *state)
{
  switch (key)
    {
    case 'p':
      {
	ifstream file(arg);
	if (!file)
	  argp_failure(state, EX_NOINPUT, errno, "cannot read sampling plan %s", arg);
	file.exceptions(ios::badbit);

	typedef istream_iterator<pair<SiteCoords, double> > reader;
	copy(reader(file), reader(), inserter(plan, plan.end()));

	return 0;
      }

    default:
      return ARGP_ERR_UNKNOWN;
    }
}


static void
processCommandLine(int argc, char *argv[])
{
  static const argp_option options[] = {
    { "plan", 'p', "FILE", 0, "downsample according to sampling plan in FILE", 0 },
    { 0, 0, 0, 0, 0, 0 }
  };

  static const argp_child children[] = {
    { &CompactReport::argp, 0, 0, 0 },
    { 0, 0, 0, 0 }
  };

  static const argp argp = {
    options, parseFlag, 0, 0, children, 0, 0
  };

  argp_parse(&argp, argc, argv, 0, 0, 0);
}


////////////////////////////////////////////////////////////////////////
//
//  the main event
//


int
main(int argc, char *argv[])
{
  set_terminate(__gnu_cxx::__verbose_terminate_handler);
  processCommandLine(argc, argv);

  ifstream entropy;
  entropy.exceptions(ios::badbit);
  entropy.open("/dev/urandom");
  entropy.read(reinterpret_cast<char *>(&gsl_rng_default_seed), sizeof(gsl_rng_default_seed));

  generator = gsl_rng_alloc(gsl_rng_taus);
  yylex();
  gsl_rng_free(generator);

  return 0;
}
