/*-*- c -*-*/

%{
#include <errno.h>
#include <gsl/gsl_randist.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static gsl_rng *generator;
static double probability;
%}


%option nounput
%option noyywrap

%x COUNTS


%%


<INITIAL>^[0-9]+\t[0-9]+\t	{
  fputs(yytext, stdout);
  BEGIN(COUNTS);
}

<COUNTS>[1-9][0-9]*	{
  /* above pattern matches nonzero counts only */
  /* zero counts pass through unchanged */
  const unsigned complete = strtoul(yytext, 0, 10);
  const unsigned reduced = gsl_ran_binomial(generator, probability, complete);
  printf("%u", reduced);
}

<COUNTS>\n	{
  fputc('\n', stdout);
  BEGIN(INITIAL);
}


%%


static const char *arg0;


static void usage()
{
  fprintf(stderr, "Usage: %s <sparsity> [<seed>]\n", arg0);
  exit(2);
}


static unsigned parseUnsigned(const char *arg)
{
  char *remainder;
  const unsigned result = strtoul(arg, &remainder, 0);

  if (*remainder)
    usage();

  return result;
}


int main(int argc, char *argv[])
{
  unsigned sparsity;

  arg0 = argv[0];

  switch (argc)
    {
    case 2:
      {
	FILE * const entropy = fopen("/dev/urandom", "rb");
	if (!entropy)
	  {
	    fprintf(stderr, "%s: cannot open system entropy: %s\n", arg0, strerror(errno));
	    exit(1);
	  }

	if (fread(&gsl_rng_default_seed, sizeof(gsl_rng_default_seed), 1, entropy) != 1)
	  {
	    fprintf(stderr, "%s: cannot read system entropy: %s\n", arg0, strerror(errno));
	    exit(1);
	  }

	fclose(entropy);
      }
      break;

    case 3:
      gsl_rng_default_seed = parseUnsigned(argv[2]);
      break;

    default:
      usage();
    }

  generator = gsl_rng_alloc(gsl_rng_taus);

  sparsity = parseUnsigned(argv[1]);
  if (sparsity == 0)
    usage();
  probability = 1. / sparsity;

  yylex();

  gsl_rng_free(generator);
  return 0;
}
