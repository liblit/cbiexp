#include <argp.h>
#include <iostream>
#include <map>
#include "classify_runs.h"
#include "sites.h"
#include "units.h"
#include "utils.h"

using namespace std;


typedef map<char, unsigned> Tally;


////////////////////////////////////////////////////////////////////////
//
//  Command line processing
//


static const char *experimentName = "dummy";
static const char *predStatsName = "preds.txt";
static int confidence = -1;
static const char *sourceDirectory = 0;

static const char *successList = "s.runs";
static const char *failureList = "f.runs";


static const argp_option options[] = {
  {
    "experiment",
    'e',
    "NAME",
    0,
    "human-readable name of experiment",
    0
  },
  {
    "predicates",
    'p',
    "preds.txt",
    0,
    "predicate statistics file",
    0
  },
  {
    "confidence",
    'c',
    "##",
    0,
    "experiment used ##% confidence",
    0
  },
  {
    "source-directory",
    'd',
    "SRC-DIR",
    0,
    "hyperlink to program sources in SRC-DIR",
    0
  },
  {
    "success-list",
    's',
    "s.runs",
    0,
    "file listing successful runs",
    0
  },
  {
    "failure-list",
    'f',
    "f.runs",
    0,
    "file listing failed runs",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};


static int
parseFlag(int key, char *arg, argp_state *state)
{
  switch (key)
    {
    case 'e':
      experimentName = arg;
      return 0;
    case 'p':
      predStatsName = arg;
      return 0;
    case 'c':
      char *tail;
      confidence = strtol(arg, &tail, 0);
      if (errno || *tail || confidence <= 0 || confidence > 100)
	argp_error(state, "invalid confidence level \"%s\"", arg);
      return 0;
    case 'd':
      sourceDirectory = arg;
      return 0;
    case 's':
      successList = arg;
      return 0;
    case 'f':
      failureList = arg;
      return 0;
    default:
      return ARGP_ERR_UNKNOWN;
    }
}


static const argp argp = {
  options, parseFlag, 0, 0, 0, 0, 0
};



////////////////////////////////////////////////////////////////////////
//
//  XML printing functions
//


static void
print_summary_scheme(ostream &out, const char name[], size_t total, size_t retain)
{
  out << "<scheme name=\"" << name
      << "\" total=\"" << total
      << "\" retain=\"" << retain
      << "\"/>";
}


static void
print_summary(ostream &out, Tally &tally)
{
  time_t now;
  time(&now);

  out << "<?xml version=\"1.0\"?>"
      << "<?xml-stylesheet type=\"text/xsl\" href=\"summary.xsl\"?>"
      << "<!DOCTYPE experiment SYSTEM \"summary.dtd\">"

      << "<experiment title=\"" << experimentName
      << "\" date=\"" << ctime(&now);

  if (sourceDirectory)
    out << "\" source-dir=\"" << sourceDirectory;

  out << "\">"

      << "<runs success=\"" << num_sruns
      << "\" failure=\"" << num_fruns
      << "\" ignore=\"" << num_runs - (num_sruns + num_fruns)
      << "\"/>"

      << "<analysis confidence=\"" << confidence << "\"/>"

      << "<schemes>";

  print_summary_scheme(out, "branches", NumBPreds, tally['B']);
  print_summary_scheme(out, "returns", NumRPreds, tally['R']);
  print_summary_scheme(out, "scalar-pairs", NumSPreds, tally['S']);
  print_summary_scheme(out, "g-object-unref", NumGPreds, tally['G']);

  out << "</schemes></experiment>\n";
}


////////////////////////////////////////////////////////////////////////
//
//  main
//


int
main(int argc, char** argv)
{
  argp_parse(&argp, argc, argv, 0, 0, 0);
  if (confidence <= 0)
    {
      cerr << "must specify experiment confidence level\n";
      argp_help(&argp, stdout, ARGP_HELP_STD_ERR, argv[0]);
    }

  ios::sync_with_stdio(false);

  classify_runs(successList, failureList);

  Tally tally;
  {
    FILE *predStats = fopen_read(predStatsName);

    pred_info info;
    while (read_pred_full(predStats, info))
      ++tally[sites[info.site].scheme_code];

    fclose(predStats);
  }

  print_summary(cout, tally);

  return 0;
}
