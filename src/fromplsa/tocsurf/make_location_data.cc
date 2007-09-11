/******************************************************************************
* From preds.txt and static-sites-info.so derive information suitable for 
* codesurfer.
******************************************************************************/

#include <argp.h>
#include <cassert>
#include <cstdio>
#include <fstream>
#include "../../site_t.h"
#include "../../Progress/Bounded.h"
#include "../../PredStats.h"
#include "../../SiteCoords.h"
#include "../../StaticSiteInfo.h"
#include "../../fopen.h"
#include "../../termination.h"
#include "../../utils.h"

using namespace std;

static auto_ptr<StaticSiteInfo> staticSiteInfo;

static void process_cmdline(int argc, char **argv)
{
    static const argp_child children[] = {
	{ 0, 0, 0, 0 }
    };

    static const argp argp = {
	0, 0, 0, 0, children, 0, 0
    };

    argp_parse(&argp, argc, argv, 0, 0, 0);
};

void print_locations(FILE* out, site_t site) 
{
    fprintf(out, "%s %s %u\n", site.file, site.fun, site.line);  
}

int main(int argc, char** argv)
{
    set_terminate_verbose();
    process_cmdline(argc, argv);

    staticSiteInfo.reset(new StaticSiteInfo());

    {
	const unsigned numPreds = PredStats::count();
	Progress::Bounded progress("reading interesting predicate list", numPreds);
	FILE * const pfp = fopenRead(PredStats::filename);
        FILE * const out = fopenWrite("pred_sites.txt");
	pred_info pi;
	while (read_pred_full(pfp, pi)) {
	    progress.step();
            const site_t site = staticSiteInfo->site(pi);
            print_locations(out, site); 
	}
	fclose(pfp);
        fclose(out);
    }

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
