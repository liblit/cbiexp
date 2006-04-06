#ifndef _RUNBUGS_H_
#define _RUNBUGS_H_

#include <list>
#include <vector>


typedef std::list<unsigned> RunList;

namespace RunBugs
{
  typedef std::list<unsigned> BugList;
  typedef std::vector<unsigned> BugVec;
  typedef std::vector<BugList> RunBugs;
  extern RunBugs runbugs;
  extern int numbugs;
  extern char * runbugsFilename;

  extern void read_runbugs();
  extern void bug_hist (BugVec &bugs, const RunList &runs);
}

#endif
