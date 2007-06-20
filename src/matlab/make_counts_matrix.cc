/******************************************************************************
* Print out a representation of the data that is easily read by matlab into a
* sparse matrix format.
*
* We do not read any data from runs that were not classified, i.e., are neither
* failures or successes. This saves us trouble and leaves zero columns as place
* holders for these runs.
*
* The files produced are Xtru.sparse which contains tru counts, Xobs.sparse 
* which contains observed counts, and X.dimensions which contains the number of
* runs and predicates in the data. This data is not always obtainable from the 
* Xtru.sparse and Xobs.sparse files. Consider the case where all counts are 
* zero. In that case Xtru.sparse and Xobs.sparse are empty, but the number of 
* runs and predicates could be anything.
*
* Currently counts are only taken for predicates in preds.txt. 
******************************************************************************/

#include <argp.h>
#include <cassert>
#include <cstdio>
#include <ext/hash_map>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
#include <vector>
#include "../CompactReport.h"
#include "../NumRuns.h"
#include "../PredStats.h"
#include "../Progress/Bounded.h"
#include "../ReportReader.h"
#include "../RunsDirectory.h"
#include "../SiteCoords.h"
#include "../classify_runs.h"
#include "../fopen.h"
#include "../termination.h"
#include "../utils.h"

using namespace std;
using __gnu_cxx::hash_map;

struct PredInfo
{
    vector < pair <unsigned int, unsigned int> > tru;
    vector < pair <unsigned int, unsigned int> > obs;
};


class PredInfos : public hash_map<PredCoords, PredInfo>
{
};


static PredInfos predInfos;

class Reader : public ReportReader
{
public:
    Reader(unsigned);

protected:
    void handleSite(const SiteCoords &, vector<count_tp> &);

private:
    void notice(const SiteCoords &coords, unsigned, unsigned, unsigned) const;

    const unsigned index;
};


inline
Reader::Reader(unsigned index)
    : index(index)
{
}


void
Reader::notice(const SiteCoords &site, unsigned predicate, unsigned int count, unsigned int sum) const
{
    if(sum == 0) return;
    const PredCoords coords(site, predicate);
    const PredInfos::iterator found = predInfos.find(coords);
    if (found != predInfos.end()) {
	PredInfo &info = found->second;
        info.obs.push_back(pair<unsigned int, unsigned int> (index, sum));
        if(count == 0) return;
	info.tru.push_back(pair<unsigned int, unsigned int> (index, count));
    }
}


void
Reader::handleSite(const SiteCoords &coords, vector<count_tp> &counts)
{
    const count_tp sum = accumulate(counts.begin(), counts.end(), (count_tp) 0);
    assert(sum > 0);

    const size_t size = counts.size();
    if (size == 2)
	for (unsigned predicate = 0; predicate < size; ++predicate) {
	    notice(coords, predicate, counts[predicate], sum);
        }
    else
	for (unsigned predicate = 0; predicate < size; ++predicate) {
	    notice(coords, 2 * predicate,           counts[predicate], sum);
	    notice(coords, 2 * predicate + 1, sum - counts[predicate], sum);
	}
}


static void process_cmdline(int argc, char **argv)
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
};


void print_sparse(ostream & out, unsigned int row, vector <pair <unsigned int, unsigned int> > cols)
{
    for(unsigned int i = 0; i < cols.size(); i++) {
        out << row << " " << cols[i].first << " " << cols[i].second << "\n";
    }
}

int main(int argc, char** argv)
{
    set_terminate_verbose();
    process_cmdline(argc, argv);

    classify_runs();

    // interesting predicates in "preds.txt" order
    typedef queue<const PredInfo *> InterestingPreds;
    InterestingPreds interesting;

    {
	const unsigned numPreds = PredStats::count();
	Progress::Bounded progress("reading interesting predicate list", numPreds);
	FILE * const pfp = fopenRead(PredStats::filename);
	pred_info pi;
	while (read_pred_full(pfp, pi)) {
	    progress.step();
	    const PredInfo &info = predInfos[pi];
	    interesting.push(&info);
	}
	fclose(pfp);
    }

    {
	Progress::Bounded progress("finding counts", NumRuns::count());
	for (unsigned runId = NumRuns::begin; runId < NumRuns::end; ++runId) {
	    progress.step();
            if (is_srun[runId] || is_frun[runId]) { 
                Reader(runId + 1 - NumRuns::begin).read(runId);
            }
	}
    }

    {
        ofstream outtru("Xtru.sparse");
        ofstream outobs("Xobs.sparse");
	Progress::Bounded progress("printing matrix", predInfos.size());
        unsigned int row = 0;
	while (!interesting.empty()) {
            row++;
	    progress.step();
	    const PredInfo * const info = interesting.front();
            print_sparse(outtru, row, info->tru);
            print_sparse(outobs, row, info->obs);
	    interesting.pop();
	}
        outtru.close();
        outobs.close();
    }

    //We record the dimensions of the data matrix. We don't want zero
    //entries on the right hand side to cause the matrix to be truncated. 
    //count() returns exactly the difference between the start and end
    //specified, so the matrix may include all zero columns corresponding
    //to discarded runs.
    {
        ofstream out("X.dimensions");
        out <<  PredStats::count() << " " << NumRuns::count() << "\n";  
        out.close();
    }

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
