#include <argp.h>
#include <cassert>
#include <cmath>
#include <ext/hash_map>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <queue>
#include <string>
#include <vector>
#include "CompactReport.h"
#include "fopen.h"
#include "NumRuns.h"
#include "PredCoords.h"
#include "PredStats.h"
#include "Progress/Bounded.h"
#include "ReportReader.h"
#include "RunsDirectory.h"
#include "SiteCoords.h"
#include "classify_runs.h"
#include "utils.h"

using namespace std;
using __gnu_cxx::hash_map;

static ofstream datfp("X.dat", ios_base::trunc);
static ofstream notdatfp("notX.dat", ios_base::trunc);

static vector<PredCoords> predVec;
static vector<PredCoords> notpredVec;

/****************************************************************
 * Predicate-specific information
 ***************************************************************/

struct PredInfo
{
    count_tp tru;
    PredInfo() {
        tru = 0;
    }
};

class PredInfos : public hash_map<PredCoords, PredInfo> {
public:
    void reset() {
        for(PredInfos::iterator c = begin(); c != end(); ++c) {
            (c->second).tru = 0;
        }
    }
      
};

static PredInfos predInfos;

inline ostream &
operator<< (ostream &out, const PredInfo &pi)
{
    out << pi.tru;
    return out;
}

inline ostream &
operator<< (ostream &out, const PredInfos &hash)
{
  for (PredInfos::const_iterator c = hash.begin(); c != hash.end(); ++c) 
  {
    out << c->first << '\n' << c->second << endl;
  }
  return out;
}

/****************************************************************
 * Information about each run
 ***************************************************************/

class Reader : public ReportReader
{

protected:
  void handleSite(const SiteCoords &, vector<count_tp> &);

private:
  void update(const SiteCoords &, unsigned, bool) const;
};

void Reader::update(const SiteCoords &site, unsigned predicate, bool tru) const
{
    const PredCoords coords(site, predicate);
    const PredInfos::iterator found = predInfos.find(coords);
    if (found != predInfos.end()) {
    	(found->second).tru = tru;    
    }
}

void Reader::handleSite(const SiteCoords &coords, vector<count_tp> &counts)
{
    const size_t size = counts.size();
    const count_tp sum = accumulate(counts.begin(), counts.end(), (count_tp) 0);
    assert(sum > 0);
    if (size == 2)
	for (unsigned predicate = 0; predicate < size; ++predicate)
	    update(coords, predicate,           counts[predicate]);
    else {
	for (unsigned predicate = 0; predicate < size; ++predicate) {
	    update(coords, 2 * predicate,           counts[predicate]);
	    update(coords, 2 * predicate + 1, sum - counts[predicate]);
	}
    }
}

/****************************************************************************
 * Utilities
 ***************************************************************************/

PredCoords
notP (PredCoords &p) {
  PredCoords notp(p);
  if (p.predicate % 2 == 0) {
    notp.predicate = p.predicate + 1;
  }
  else {
    notp.predicate = p.predicate - 1;
  }

  return notp;
}

void
read_preds()
{
  queue<PredCoords> predQueue;
  queue<PredCoords> notpredQueue;
  pred_info pi;

  FILE * const pfp = fopenRead(PredStats::filename);
  while (read_pred_full(pfp,pi)) {
      predQueue.push(pi);
      predInfos[pi] = *(new PredInfo());

      PredCoords notp = notP(pi);
      notpredQueue.push(notp);
      predInfos[notp] = *(new PredInfo());
  }
  fclose(pfp);

  predVec.resize(predQueue.size());
  int i = 0;
  while (!predQueue.empty()) {
      predVec[i++] = predQueue.front();
      predQueue.pop();
  }

  notpredVec.resize(notpredQueue.size());
  i = 0;
  while (!notpredQueue.empty()) {
      notpredVec[i++] = notpredQueue.front();
      notpredQueue.pop();
  }

  assert(notpredVec.size() == predVec.size());
}

/****************************************************************************
 * Print results to file
 ***************************************************************************/
static ostream &
operator<< (ostream &out, const vector<PredCoords> &pv)
{
    for (unsigned i = 0; i < pv.size(); ++i) {
	PredInfos::iterator found = predInfos.find(pv[i]);
	assert(found != predInfos.end());
	out << (found->second).tru << ' ';
    }
    return out;
}

void print_results()
{
  datfp << predVec << endl;
  notdatfp << notpredVec << endl;
}


/****************************************************************************
 * Processing command line options
 ***************************************************************************/

void process_cmdline(int argc, char** argv)
{
    static const argp_child children[] = {
	{ &CompactReport::argp, 0, 0, 0 },
	{ &NumRuns::argp, 0, 0, 0 },
	{ &ReportReader::argp, 0, 0, 0 },
	{ &RunsDirectory::argp, 0, 0, 0 },
	{ 0, 0, 0, 0 }
    };

    static const argp argp = {
	0, 0, 0, 0, children, 0, 0
    };

    argp_parse(&argp, argc, argv, 0, 0, 0);
}


/****************************************************************************
 * MAIN
 ***************************************************************************/

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);

    classify_runs();

    read_preds();

    datfp << scientific << setprecision(12);
    notdatfp << scientific << setprecision(12);

    Progress::Bounded prog("Recording truth values", NumRuns::count());
    for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
    	if (!is_srun[r] && !is_frun[r])
		continue;

	prog.step();

        predInfos.reset();

	Reader().read(r);

        print_results();
    }

    datfp.close();
    notdatfp.close();
    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
