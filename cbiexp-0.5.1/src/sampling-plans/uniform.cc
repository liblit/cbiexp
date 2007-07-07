#include <argp.h>
#include <sysexits.h>
#include "../StaticSiteInfo.h"

using namespace std;


static double rate;


static error_t
parseArg(int key, char *arg, argp_state *state)
{
  switch (key)
    {
    case ARGP_KEY_ARG:
      if (state->arg_num > 0)
	argp_usage(state);
 
      char *tail;
      rate = strtod(arg, &tail);
      if (errno || *tail || rate <= 0 || rate >= 1)
	argp_failure(state, EX_USAGE, errno, "invalid sampling rate \"%s\"", arg);
      return 0;

    case ARGP_KEY_NO_ARGS:
      argp_usage(state);

    default:
      return ARGP_ERR_UNKNOWN;
    }
}


static void
processCommandLine(int argc, char *argv[])
{
  static const argp argp = {
    0, parseArg, "RATE", 0, 0, 0, 0
  };

  argp_parse(&argp, argc, argv, 0, 0, 0);
}


int main(int argc, char *argv[])
{
  set_terminate(__gnu_cxx::__verbose_terminate_handler);
  processCommandLine(argc, argv);
  ios::sync_with_stdio(false);

  const StaticSiteInfo info;
  SiteCoords coords;
  for (unsigned unitNum = 0; unitNum < info.unitCount; ++unitNum)
    {
      const unit_t &unit = info.unit(unitNum);
      for (unsigned siteNum = 0; siteNum < unit.num_sites; ++siteNum)
	cout << unitNum << '\t' << siteNum << '\t' << rate << '\n';
    }

  return 0;
}
