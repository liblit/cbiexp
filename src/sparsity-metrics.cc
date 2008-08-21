#include <argp.h>
#include <fstream>
#include <iostream>
#include "CompactReport.h"
#include "NumRuns.h"
#include "Progress/Bounded.h"
#include "Progress/tty.h"
#include "RunsDirectory.h"

using namespace std;


static void
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

  argp_parse(&argp, argc, argv, 0, 0, 0);
}


int main(int argc, char *argv[])
{
  set_terminate(__gnu_cxx::__verbose_terminate_handler);
  processCommandLine(argc, argv);
  ios::sync_with_stdio(false);

  unsigned max = 0;
  unsigned nonzeros = 0;

  {
    Progress::Bounded progress("tabulating nonzeros", NumRuns::count());
    for (unsigned runId = NumRuns::begin(); runId < NumRuns::end(); ++runId)
      {
	progress.step();
	ifstream report(CompactReport::format(runId).c_str());
	report.exceptions(ios::badbit);

	while (true)
	  {
	    unsigned unitIndex, siteOffset;
	    report >> unitIndex >> siteOffset;
	    if (report.eof())
	      break;

	    while (report.peek() != '\n')
	      {
		unsigned count;
		report >> count;
		if (count)
		  {
		    if (count > max) max = count;
		    ++nonzeros;
		  }
	      }
	  }

	Progress::tty << "  (" << nonzeros << ", " << max << ") " << flush;
      }
  }

  cout << "max: " << max << '\n'
       << "nonzeros: " << nonzeros << '\n';
}
