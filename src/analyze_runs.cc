#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator>
#include <libgen.h>
#include <string>
#include <unistd.h>
#include "Confidence.h"
#include "MapSites.h"
#include "NumRuns.h"
#include "RunsDirectory.h"
#include "SourceDirectory.h"
#include "Stylesheet.h"
#include "Verbose.h"
#include "compiler.h"
#include "shell.h"

using namespace std;


const char *Stylesheet::filename = "view.xsl";


static string sites_txt_files;


////////////////////////////////////////////////////////////////////////
//
//  Command line processing
//


enum {
    DoProcessLabels,
    DoMapSites,
    DoConvertReports,
    DoComputeResults,
    DoComputeObsTru,
    DoPrintSummary,
    DoPrintResults1,
    // DoPrintResultsN,

    PhaseCount
};


static char * const phaseNames[PhaseCount + 1] = {
    "process-labels",
    "map-sites",
    "convert-reports",
    "compute-results",
    "compute-obs-tru",
    "print-summary",
    "print-results-1",
    // "print-results-n",
    0
};


static bool phaseSelected[PhaseCount];


static int parse_flag(int key, char *arg, argp_state *state)
{
    switch (key) {
    case 'd':
	do {
	    char *value;
	    const int index = getsubopt(&arg, phaseNames, &value);
	    if (index == -1) {
		argp_error(state, "no such phase: %s", value);
		assert(0);
	    } else if (value) {
		argp_error(state, "no such phase: %s=%s", arg, value);
		assert(0);
	    } else {
		assert(index < PhaseCount);
		phaseSelected[index] = true;
	    }
	} while (*arg);
	return 0;
    case 's':
	sites_txt_files += arg;
	sites_txt_files += ' ';
	return 0;
    case 'h':
	copy(phaseNames, phaseNames + PhaseCount, ostream_iterator<const char *>(cout, "\n"));
	exit(0);
    default:
	return ARGP_ERR_UNKNOWN;
    }
}


static void process_cmdline(int argc, char *argv[])
{
    static const argp_option options[] = {
	{ "do", 'd', "PHASES", 0, "perform the comma-delimited list of PHASES; can be given multiple times", 0 },
	{ "sites-text", 's', "FILE", 0, "read raw static site information from FILE; can be given multiple times", 0 },
	{ "help-phases", 'h', 0, 0, "list known phases for use with \"--do\" flag", -1 },
	{ 0, 0, 0, 0, 0, 0 }
    };

    static const argp_child children[] = {
	{ &Confidence::argp, 0, 0, 0 },
	{ &NumRuns::argp, 0, 0, 0 },
	{ &RunsDirectory::argp, 0, 0, 0 },
	{ &SourceDirectory::argp, 0, 0, 0 },
	{ &Stylesheet::argp, 0, 0, 0 },
	{ &Verbose::argp, 0, 0, 0 },
	{ 0, 0, 0, 0 }
    };

    static const argp argp = {
	options, parse_flag, 0, 0, children, 0, 0
    };

    argp_parse(&argp, argc, argv, 0, 0, 0);
}


static void add_link(const char srcdir[], const char basename[], const char suffix[])
{
    string filename(basename);
    filename += '.';
    filename += suffix;

    string oldpath(srcdir);
    oldpath += '/';
    oldpath += filename;

    const string &newpath(filename);

    assert(srcdir != ".");
    unlink(newpath.c_str());

    if (symlink(oldpath.c_str(), newpath.c_str()) != 0) {
	const int code = errno;
	cerr << "symlink from " << oldpath << " to " << newpath << " failed: " << strerror(code) << '\n';
	exit(code || 1);
    }
}

static void add_links(const char srcdir[], const char basename[])
{
    static const char * const suffixes[] = {"css", "dtd", "xsl"};
    for (unsigned i = 0; i < sizeof(suffixes) / sizeof(*suffixes); ++i)
	add_link(srcdir, basename, suffixes[i]);
}

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);
    const char * const bindir = dirname(argv[0]);
    const char * const objdir = bindir;
    const char * const incdir = bindir;
    bool needLogoLinks = false;

    if (phaseSelected[DoProcessLabels]) {
	puts("Processing run labels ...");
	shell("%s/process_run_labels --number-of-runs=%u --runs-directory=%s",
	      bindir, NumRuns::value(), RunsDirectory::root);
    }

    if (phaseSelected[DoMapSites]) {
	puts("Mapping sites ...");
	shell("%s/map_sites.sed %s | %s/map_sites",
	      bindir, sites_txt_files.c_str(), bindir);
	shell("%s -I%s -c %s.cc", compiler, incdir, MapSites::sitesBasename);
	shell("%s -I%s -c %s.cc", compiler, incdir, MapSites::unitsBasename);
    }

    if (phaseSelected[DoConvertReports]) {
	puts("Converting reports ...");
	shell("%s %s/convert_reports.o %s.o -L%s -lanalyze -o convert-reports %s",
	      linker, objdir, MapSites::unitsBasename, objdir, lexlib);
	shell("./convert-reports --runs-directory=%s", RunsDirectory::root);
    }

    if (phaseSelected[DoComputeResults]) {
	puts("Computing results ...");
	add_links(incdir, "summary");
	shell("%s %s/compute_results.o %s.o %s.o -L%s -lanalyze -o compute-results",
	      linker, objdir, MapSites::sitesBasename, MapSites::unitsBasename, objdir);
	shell("./compute-results --confidence=%u --runs-directory=%s",
	      Confidence::level, RunsDirectory::root);
    }

    if (phaseSelected[DoComputeObsTru]) {
	puts("Computing obs and tru ...");
	shell("%s %s/compute_obs_tru.o %s.o %s.o -L%s -lanalyze -o compute-obs-tru",
	      linker, objdir, MapSites::sitesBasename, MapSites::unitsBasename, objdir);
	shell("./compute-obs-tru --runs-directory=%s", RunsDirectory::root);
    }

    if (phaseSelected[DoPrintSummary]) {
	puts("Printing summary ...");
	add_links(incdir, "summary");
	add_link(incdir, "sorts", "dtd");
	add_link(incdir, "sorts", "xml");
	shell("%s %s/gen_summary.o %s.o %s.o -L%s -lanalyze -o gen-summary",
	      linker, objdir, MapSites::sitesBasename, MapSites::unitsBasename, objdir);
	shell("./gen-summary --confidence=%u --source-directory=%s >summary.xml",
	      Confidence::level, SourceDirectory::root);
	needLogoLinks = true;
    }

    if (phaseSelected[DoPrintResults1]) {
	puts("Pretty-printing results-1 ...");
	add_links(incdir, "view");
	shell("%s %s/gen_views.o %s.o %s.o -L%s -lanalyze -L%s/Score -lScore -o gen-views",
	      linker, objdir, MapSites::sitesBasename, MapSites::unitsBasename,
	      objdir, objdir);
	shell("./gen-views --stylesheet=%s", Stylesheet::filename);
	needLogoLinks = true;
    }

    if (needLogoLinks) {
	add_link(incdir, "logo", "css");
	add_link(incdir, "logo", "xsl");
    }

/*
    if (phaseSelected[DoPrintResultsN]) {
	REQUIRE("-do-print-results-n", "-r" , result_summary_xml_file);
	REQUIRE("-do-print-results-n", "-ps", preds_src_file);
	REQUIRE("-do-print-results-n", "-ka", all_cluster_txt_file);
	REQUIRE("-do-print-results-n", "-kp", per_cluster_txt_file);
	puts("Pretty-printing results-n ...");
	shell("%s %s/gen_preds_file.o %s.o -o %s", linker, objdir, preds_src_file, GEN_PREDS_FILE);
	FILE* all_fp = fopen(all_cluster_txt_file, "r");
	assert(all_fp);
	while (1) {
	    int x, y;
	    fscanf(all_fp, "%d %d", &x, &y);
	    if (feof(all_fp))
		break;
	    char file[1000];
	    sprintf(file, per_cluster_txt_file, x);
	    shell("%s < %s > preds.%d.txt", GEN_PREDS_FILE, file, x);
	    sprintf(file, "preds.%d.txt", x);
	    gen_views(file, x);
	}
	fclose(all_fp);
    }
*/

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
