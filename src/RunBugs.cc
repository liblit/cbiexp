#include "RunBugs.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include "NumRuns.h"
#include "fopen.h"

namespace RunBugs
{
  int numbugs = 10;
  const char * runbugsFilename = "run-bugs.txt";
  RunBugs runbugs;
  
  // read in the list of failed runs and their bugs
  void read_runbugs ()
  {
    const unsigned numRuns = NumRuns::end;
    runbugs.resize(numRuns);
    for (unsigned i = 0; i < numRuns; ++i) {
      runbugs[i].clear();
    }
  
    FILE * runfp = fopenRead(runbugsFilename);
    assert(runfp);
    char buf[1024];
    char *pos;
    int r;
    while (true) {
      fgets(buf, 1024, runfp);
      if (feof(runfp))
        break;
  
      pos = strtok(buf, "\t");
      r = atoi(pos);
      pos = strtok(NULL, "\t\n");
      while (pos != NULL) {
        runbugs[r].push_back(atoi(pos));
        pos = strtok(NULL, "\t\n");
      }
    }
  
    fclose(runfp);
  }
  
  // aggregate the bug histogram correponsing to a list of runs
  void
  bug_hist (BugVec &bugs, const RunList &runs)
  {
    bugs.clear();
    bugs.resize(numbugs);
    for (RunList::const_iterator c = runs.begin(); c != runs.end(); ++c) {
      const BugList &buglist = runbugs[(*c)];
      for (BugList::const_iterator d = buglist.begin();
           d != buglist.end(); ++d) {
        int b = (*d);
        assert(b >= 1 && b <= numbugs);
        bugs[b-1] += 1;
      }
    }
  }
}
