#include <fstream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "Score/Fail.h"
#include "Score/HarmonicMean.h"
#include "Score/Increase.h"
#include "Score/LowerBound.h"
#include "Score/TrueInFails.h"
#include "generate-view.h"
#include "sorts.h"
#include "sites.h"
#include "utils.h"

using namespace std;


typedef map<string, Stats> StatsMap;


char* preds_txt_file = "preds.txt";


static void
process_cmdline(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
	if (!strcmp(argv[i], "-p")) {
	    i++;
	    preds_txt_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-h")) {
	    puts("Usage: gen-views [-p <preds.txt>]");
	    exit(0);
	}
	printf("Illegal option: %s\n", argv[i]);
	exit(1);
    }
}


template <template <class Get> class Sorter, class Get>
static void
gen_permutation(const string &scheme, const Stats &stats)
{
    // make sure we will be able to write before we spend time sorting
    ostringstream filename;
    filename << scheme << '_' << Get::code << ".xml";
    ofstream xml;
    xml.exceptions(ios::eofbit | ios::failbit | ios::badbit);
    xml.open(filename.str().c_str());

    // build an initial identity permutation
    const size_t size = stats.size();
    Permutation permutation;
    permutation.reserve(size);
    for (unsigned index = 0; index < size; ++index)
	permutation.push_back(index);

    // sort that permutation using the given sorter
    Sorter<Get> sorter(stats);
    sort(permutation.begin(), permutation.end(), sorter);

    // generate XML
    xml << "<?xml version=\"1.0\"?>"
	<< "<?xml-stylesheet type=\"text/xsl\" href=\"view.xsl\"?>"
	<< "<!DOCTYPE view SYSTEM \"view.dtd\">"
	<< "<view scheme=\"" << scheme
	<< "\" sort=\"" << Get::code << "\">";

    for (Permutation::const_iterator index = permutation.begin();
	 index != permutation.end(); ++index)
	xml << stats[*index];

    xml << "</view>\n";
}


static void
gen_permutations(const string &scheme, const Stats &stats)
{
    using namespace Sort;
    using namespace Score;

    gen_permutation<Descending, LowerBound>(scheme, stats);
    gen_permutation<Descending, Increase>(scheme, stats);
    gen_permutation<Descending, Fail>(scheme, stats);
    gen_permutation<Descending, TrueInFails>(scheme, stats);
    // gen_permutation<Descending, HarmonicMean>(scheme, stats);
}


int
main(int argc, char** argv)
{
    process_cmdline(argc, argv);

    // group predicates by scheme for easier filtering later
    // scheme code "all" lists predicates for all schemes
    StatsMap stats_map;

    // load up all of the predicates with computed statistics
    FILE *stats_file = fopen_read(preds_txt_file);
    pred_info info;
    while (read_pred_full(stats_file, info)) {
	static const string all("all");
	stats_map[all].push_back(info);
	stats_map[scheme_name(sites[info.site].scheme_code)].push_back(info);
    }

    // generate sorted views for each individual scheme
    for (StatsMap::const_iterator stats = stats_map.begin();
	 stats != stats_map.end(); ++stats)
	gen_permutations(stats->first, stats->second);

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
