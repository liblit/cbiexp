#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <libgen.h>
#include <string>
#include "compiler.h"
#include "shell.h"

#define PROCESS_RUN_LABELS "process_run_labels"
#define MAP_SITES          "map_sites"
#define CONVERT_REPORTS    "./convert-reports"
#define COMPUTE_RESULTS    "./compute-results"
#define COMPUTE_OBS_TRU    "./compute-obs-tru"
#define GEN_VIEWS          "./gen-views"
#define GEN_PREDS_FILE     "./gen-preds-file"

#define DEFAULT_EXPERIMENT_NAME "dummy"
#define DEFAULT_PROGRAM_SRC_DIR "."
#define DEFAULT_CONFIDENCE "95"

/* Default files used by the phases for communication.
 * Convention:
 * <...>_TXT_FILE means text file
 * <...>_SRC_FILE means C++ file 
 * <...>_HTM_FILE means HTML file
 */

#define DEFAULT_SRUNS_TXT_FILE "s.runs"
#define DEFAULT_FRUNS_TXT_FILE "f.runs"
#define DEFAULT_SITES_SRC_FILE "sites"
#define DEFAULT_UNITS_SRC_FILE "units"
#define DEFAULT_PREDS_TXT_FILE "preds.txt"
#define DEFAULT_RESULT_SUMMARY_HTM_FILE "summary.html"
#define DEFAULT_OBS_TXT_FILE "obs.txt"
#define DEFAULT_TRU_TXT_FILE "tru.txt"

/**************************************************************************
DO NOT EDIT ANYTHING BELOW THIS
 **************************************************************************/

using std::string;

bool do_process_labels = false;
bool do_map_sites = false;
bool do_convert_reports = false;
bool do_compute_results = false;
bool do_compute_obs_tru = false;
bool do_print_results_1 = false;
bool do_print_results_n = false;

int num_runs = -1;
char* experiment_name = DEFAULT_EXPERIMENT_NAME;
char* program_src_dir = DEFAULT_PROGRAM_SRC_DIR;
char* confidence = DEFAULT_CONFIDENCE;

char* sruns_txt_file = NULL;
char* fruns_txt_file = NULL;
string sites_txt_file;
char* sites_src_file = NULL;
char* units_src_file = NULL;
char* preds_txt_file = NULL;
char* result_summary_htm_file = NULL;
char* obs_txt_file = NULL;
char* tru_txt_file = NULL;
char* all_cluster_txt_file = NULL;
char* per_cluster_txt_file = NULL;

char* label_path_fmt = NULL;
char* verbose_report_path_fmt = NULL;
char* compact_report_path_fmt = NULL;

void process_cmdline(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {

        /*********************************************
         * phase selection options
         *********************************************/

	if (!strcmp(argv[i], "-do-process-labels")) {
	    do_process_labels = true;
	    continue;
	}
	if (!strcmp(argv[i], "-do-map-sites")) {
	    do_map_sites = true;
	    continue;
	}
	if (!strcmp(argv[i], "-do-convert-reports")) {
	    do_convert_reports = true;
	    continue;
	}
	if (!strcmp(argv[i], "-do-compute-results")) {
	    do_compute_results = true;
	    continue;
	}
	if (!strcmp(argv[i], "-do-compute-obs-tru")) {
	    do_compute_obs_tru = true;
	    continue;
	}
	if (!strcmp(argv[i], "-do-print-results-1")) {
	    do_print_results_1 = true;
	    continue;
	}
	if (!strcmp(argv[i], "-do-print-results-n")) {
	    do_print_results_n = true;
	    continue;
	}

        /*********************************************
         * high-level experiment options
         *********************************************/

	if (!strcmp(argv[i], "-n")) {
	    i++;
	    num_runs = atoi(argv[i]);
	    continue;
	}
	if (!strcmp(argv[i], "-e")) {
	    i++;
	    experiment_name = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-d")) {
	    i++;
	    program_src_dir = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-c")) {
	    i++;
	    confidence = argv[i];
	    continue;
	}

        /*********************************************
         * file-name options
         *********************************************/


	if (!strcmp(argv[i], "-s")) {
	    i++;
	    sruns_txt_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-f")) {
	    i++;
	    fruns_txt_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-st")) {
	    i++;
	    sites_txt_file += argv[i];
	    sites_txt_file += ' ';
	    continue;
	}
	if (!strcmp(argv[i], "-ss")) {
	    i++;
	    sites_src_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-us")) {
	    i++;
	    units_src_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-p")) {
	    i++;
	    preds_txt_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-r")) {
	    i++;
	    result_summary_htm_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-o")) {
	    i++;
	    obs_txt_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-t")) {
	    i++;
	    tru_txt_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-ka")) {
	    i++;
	    all_cluster_txt_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-kp")) {
	    i++;
	    per_cluster_txt_file = argv[i];
	    continue;
	}

        /*********************************************
         * path formats
         *********************************************/

	if (!strcmp(argv[i], "-l")) {
	    i++;
	    label_path_fmt = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-vr")) {
	    i++;
	    verbose_report_path_fmt = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-cr")) {
	    i++;
	    compact_report_path_fmt = argv[i];
	    continue;
	}

        /*********************************************
         * debugging
         *********************************************/

	if (!strcmp(argv[i], "-verbose")) {
	    verbose = true;
	    continue;
	}

        /*********************************************
         * help!
         *********************************************/

	if (!strcmp(argv[i], "-h")) {
	    puts("Usage: analyze-runs <options>\n"
		 "-do-process-labels\n"
		 "-do-map-sites\n"
		 "-do-convert-reports\n"
		 "-do-compute-results\n"
		 "-do-compute-obs-tru\n"
		 "-do-print-results-1\n"
		 "-do-print-results-n\n"
		 "-n   <num-runs>\n"
		 "-e   <experiment-name>\n"
		 "-d   <program-src-dir>\n"
		 "-c   <confidence>\n"
		 "-s   <sruns-txt-file>\n"
		 "-f   <fruns-txt-file>\n"
		 "-st  <sites-txt-file>\n"
		 "-ss  <sites-src-file>\n"
		 "-us  <units-src-file>\n"
		 "-pt  <preds-txt-file>\n"
		 "-ps  <preds-src-file>\n"
		 "-r   <result-summary-htm-file>\n"
		 "-o   <obs-txt-file>\n"
		 "-t   <tru-txt-file>\n"
		 "-ka  <all-cluster-txt-file>\n"
		 "-kp  <per-cluster-txt-file>\n"
		 "-l   <label-path-fmt>\n"
		 "-vr  <verbose-report-path-fmt>\n"
		 "-cr  <compact-report-path-fmt>\n"
		 "-verbose");
	    exit(0);
	}

	printf("Illegal option: %s\n", argv[i]);
	exit(1);
    }
}

void gen_views(char* preds_file, int prefix)
{
    shell("awk '{ if ($1~/B/) print $0 }' < %s > B.txt", preds_file);
    shell("awk '{ if ($1~/R/) print $0 }' < %s > R.txt", preds_file);
    shell("awk '{ if ($1~/S/) print $0 }' < %s > S.txt", preds_file);
    shell("awk '{ if ($1~/G/) print $0 }' < %s > G.txt", preds_file);

    shell("%s -r %s -p %d %s", GEN_VIEWS, result_summary_htm_file,
	  prefix, verbose ? "-verbose" : "");
}

void REQUIRE(char* main_opt, char* sub_opt, bool s)
{
    if (!s) {
        printf("When you specify %s, you must also specify %s\n", main_opt, sub_opt);
        exit(1);
    }
}

void FORBID(char* main_opt, char* sub_opt, char* s)
{
    if (s) {
        printf("When you specify %s, you must not specify %s\n", main_opt, sub_opt);
        exit(1);
    }
}

int main(int argc, char** argv)
{
    const char * const bindir = dirname(argv[0]);
    const char * const objdir = bindir;
    const char * const incdir = bindir;
    process_cmdline(argc, argv);

    if (do_process_labels) {
        REQUIRE("-do-process-labels", "-l", label_path_fmt);
        FORBID ("-do-process-labels", "-s", sruns_txt_file);
        FORBID ("-do-process-labels", "-f", fruns_txt_file);
	if (num_runs == -1) {
	    puts("When you specify -do-process-labels, you must also specify -n");
	    exit(1);
	}
	puts("Processing run labels ...");
	sruns_txt_file = DEFAULT_SRUNS_TXT_FILE;
	fruns_txt_file = DEFAULT_FRUNS_TXT_FILE;
	shell("%s/%s -n %d -l %s -s %s -f %s",
	      bindir, PROCESS_RUN_LABELS, num_runs, label_path_fmt, sruns_txt_file, fruns_txt_file);
    }

    if (do_map_sites) {
	REQUIRE("-do-map-sites", "-st", !sites_txt_file.empty());
	FORBID ("-do-map-sites", "-ss", sites_src_file);
	FORBID ("-do-map-sites", "-us", units_src_file);
	puts("Mapping sites ...");
	sites_src_file = DEFAULT_SITES_SRC_FILE;
	units_src_file = DEFAULT_UNITS_SRC_FILE;
	shell("%s/map_sites.sed %s | %s/" MAP_SITES " -ss %s.cc -us %s.cc",
	      bindir, sites_txt_file.c_str(), bindir,
	      sites_src_file, units_src_file);
	shell("%s -I%s -c %s.cc", compiler, incdir, sites_src_file);
	shell("%s -I%s -c %s.cc", compiler, incdir, units_src_file);
    }

    if (do_convert_reports) {
	REQUIRE("-do-convert-reports", "-vr", verbose_report_path_fmt);
	REQUIRE("-do-convert-reports", "-cr", compact_report_path_fmt);
	REQUIRE("-do-convert-reports", "-s" , sruns_txt_file);
	REQUIRE("-do-convert-reports", "-f" , fruns_txt_file);
	REQUIRE("-do-convert-reports", "-us", units_src_file);
	puts("Converting reports ...");
	shell("%s %s/convert_reports.o %s.o -L%s -lanalyze -o %s %s",
	      linker, objdir, units_src_file, objdir, CONVERT_REPORTS, lexlib);
	shell("%s -s %s -f %s -vr %s -cr %s",
	      CONVERT_REPORTS, sruns_txt_file, fruns_txt_file, verbose_report_path_fmt, compact_report_path_fmt);
    }

    if (do_compute_results) {
	REQUIRE("-do-compute-results", "-ss", sites_src_file);
	REQUIRE("-do-compute-results", "-us", units_src_file);
	REQUIRE("-do-compute-results", "-s" , sruns_txt_file);
	REQUIRE("-do-compute-results", "-f" , fruns_txt_file);
	REQUIRE("-do-compute-results", "-cr", compact_report_path_fmt);
	FORBID ("-do-compute-results", "-p" , preds_txt_file);
	FORBID ("-do-compute-results", "-r" , result_summary_htm_file);
	puts("Computing results ...");
	preds_txt_file = DEFAULT_PREDS_TXT_FILE;
	result_summary_htm_file = DEFAULT_RESULT_SUMMARY_HTM_FILE;
	shell("%s %s/compute_results.o %s.o %s.o -L%s -lanalyze -o %s",
	      linker, objdir, sites_src_file, units_src_file, objdir, COMPUTE_RESULTS);
	shell("%s -e %s -d %s -c %s -s %s -f %s -cr %s -p %s -r %s",
	      COMPUTE_RESULTS, experiment_name, program_src_dir, confidence,
              sruns_txt_file, fruns_txt_file, compact_report_path_fmt,
	      preds_txt_file, result_summary_htm_file);
/*
	shell("%s -I%s -c %s.cc", compiler, incdir, preds_src_file);

        shell("echo \"#include <preds.h>\" > %s.cc", preds_src_file);
        shell("echo \"const char* const preds[] = {\" >> %s.cc", preds_src_file);
        shell("awk '{ print \"\t\\\"\" $n \"\\\",\" }' %s >> %s.cc", preds_full_txt_file, preds_src_file);
        shell("echo \"};\" >> %s.cc", preds_src_file);
*/
    }

    if (do_compute_obs_tru) {
        REQUIRE("-do-compute-obs-tru", "-ss", sites_src_file);
        REQUIRE("-do-compute-obs-tru", "-us", units_src_file);
	REQUIRE("-do-compute-obs-tru", "-s" , sruns_txt_file);
	REQUIRE("-do-compute-obs-tru", "-f" , fruns_txt_file);
        REQUIRE("-do-compute-obs-tru", "-p" , preds_txt_file);
        REQUIRE("-do-compute-obs-tru", "-cr", compact_report_path_fmt);
	FORBID ("-do-compute-obs-tru", "-o" , obs_txt_file);
	FORBID ("-do-compute-obs-tru", "-t" , tru_txt_file);
	puts("Computing obs and tru ...");
	obs_txt_file = DEFAULT_OBS_TXT_FILE;
	tru_txt_file = DEFAULT_TRU_TXT_FILE;
	shell("%s %s/compute_obs_tru.o %s.o %s.o -L%s -lanalyze -o %s",
	      linker, objdir, sites_src_file, units_src_file, objdir, COMPUTE_OBS_TRU);
	shell("%s -s %s -f %s -p %s -cr %s -o %s -t %s",
	      COMPUTE_OBS_TRU, sruns_txt_file, fruns_txt_file, preds_txt_file,
              compact_report_path_fmt, obs_txt_file, tru_txt_file);
    }

    if (do_print_results_1) {
	REQUIRE("-do-print-results-1", "-r", result_summary_htm_file);
	REQUIRE("-do-print-results-1", "-p", preds_txt_file);
        REQUIRE("-do-print-results-1", "-ss", sites_src_file);
        REQUIRE("-do-print-results-1", "-us", units_src_file);
	shell("%s %s/gen_views.o %s.o %s.o -L%s -lanalyze -o %s",
	      linker, objdir, sites_src_file, units_src_file, objdir, GEN_VIEWS);
	puts("Pretty-printing results-1 ...");
	gen_views(preds_txt_file, 0);
    }

/*
    if (do_print_results_n) {
	REQUIRE("-do-print-results-n", "-r" , result_summary_htm_file);
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
