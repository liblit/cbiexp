/*-*- c++ -*-*/

%{
#include <iostream>
#include <ext/hash_set>
#include <gsl/gsl_randist.h>
#include "SiteCoords.h"

static gsl_rng *generator;
static double probability = NAN;

static unsigned unitIndex; 
__gnu_cxx::hash_set<SiteCoords> exempt;
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
  if (exempt.find(coords) == exempt.end())
    BEGIN(COUNTS);
  else
    BEGIN(EXEMPT);
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


unsigned sparsity;


static int
parseFlag(int key, char *arg, argp_state *state)
{
  switch (key)
    {
    case 'x':
      {
	ifstream file(arg);
	if (!file)
	  argp_failure(state, EX_NOINPUT, errno, "cannot read exemption list %s", arg);
	file.exceptions(ios::badbit);
	copy(istream_iterator<SiteCoords>(file), istream_iterator<SiteCoords>(), inserter(exempt, exempt.end()));
	return 0;
      }

    case 's':
      {
	char *tail;
	errno = 0;
	const unsigned sparsity = strtoul(arg, &tail, 0);
	probability = 1. / sparsity;
	if (errno || *tail || sparsity == 0 || isnan(probability))
	  argp_error(state, "invalid sparsity \"%s\"", arg);
	return 0;
      }

    case ARGP_KEY_END:
      if (isnan(probability))
	argp_failure(state, EX_USAGE, 0, "must use \"--sparsity=NUM\" flag to set target sparsity");

    default:
      return ARGP_ERR_UNKNOWN;
    }
}


static void
processCommandLine(int argc, char *argv[])
{
  static const argp_option options[] = {
    { "exempt-sites", 'x', "FILE", 0, "do not downsample sites listed in FILE (default: no exemptions)", 0 },
    { "sparsity", 's', "NUM", 0, "downsample to average sampling rate 1/NUM (no default)", 0 },
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
