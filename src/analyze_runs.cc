#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <libgen.h>
#include "compiler.h"
#include "shell.h"

#define PROCESS_RUN_LABELS "process_run_labels"
#define MAP_SITES          "map_sites"
#define CONVERT_REPORTS    "./convert-reports"
#define COMPUTE_RESULTS    "./compute-results"
#define COMPUTE_OBS_TRU    "./compute-obs-tru"
#define GEN_VIEWS          "gen_views"
#define GEN_PREDS_FILE     "./gen-preds-file"

#define DEFAULT_SRUNS_FILE "s.runs"
#define DEFAULT_FRUNS_FILE "f.runs"
#define DEFAULT_COMPACT_SITES_FILE "sites.txt"
#define DEFAULT_UNITS_FILE_BASE "units"
#define DEFAULT_EXPERIMENT_NAME "dummy"
#define DEFAULT_PROGRAM_SRC_DIR "."
#define DEFAULT_CONFIDENCE "95"
#define DEFAULT_PREDS_FULL_FILE "preds.full.txt"
#define DEFAULT_PREDS_ABBR_FILE "preds.abbr.txt"
#define DEFAULT_PREDS_FILE_BASE "preds"
#define DEFAULT_RESULT_SUMMARY_FILE "summary.html"
#define DEFAULT_OBS_FILE "obs.txt"
#define DEFAULT_TRU_FILE "tru.txt"

/**************************************************************************
DO NOT EDIT ANYTHING BELOW THIS
 **************************************************************************/

bool do_process_labels = false;
bool do_map_sites = false;
bool do_convert_reports = false;
bool do_compute_results = false;
bool do_compute_obs_tru = false;
bool do_print_results_1 = false;
bool do_print_results_n = false;

int num_runs = -1;
char* label_path_fmt = NULL;
char* sruns_file = NULL;
char* fruns_file = NULL;

char* verbose_sites_file = NULL;
char* compact_sites_file = NULL;
char* units_file_base = NULL;

char* verbose_report_path_fmt = NULL;
char* compact_report_path_fmt = NULL;

char* experiment_name = DEFAULT_EXPERIMENT_NAME;
char* program_src_dir = DEFAULT_PROGRAM_SRC_DIR;
char* confidence = DEFAULT_CONFIDENCE;
char* trace_file = NULL;
char* preds_full_file = NULL;
char* preds_abbr_file = NULL;
char* preds_file_base = NULL;
char* result_summary_file = NULL;
char* all_cluster_file = NULL;
char* per_cluster_file = NULL;

char* obs_file = NULL;
char* tru_file = NULL;

void process_cmdline(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
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
	if (!strcmp(argv[i], "-n")) {
	    i++;
	    num_runs = atoi(argv[i]);
	    continue;
	}
	if (!strcmp(argv[i], "-l")) {
	    i++;
	    label_path_fmt = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-s")) {
	    i++;
	    sruns_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-f")) {
	    i++;
	    fruns_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-vs")) {
	    i++;
	    verbose_sites_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-cs")) {
	    i++;
	    compact_sites_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-u")) {
	    i++;
	    units_file_base = argv[i];
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
	if (!strcmp(argv[i], "-t")) {
	    i++;
	    trace_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-pf")) {
	    i++;
	    preds_full_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-pa")) {
	    i++;
	    preds_abbr_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-pb")) {
	    i++;
	    preds_file_base = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-r")) {
	    i++;
	    result_summary_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-obs")) {
	    i++;
	    obs_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-tru")) {
	    i++;
	    tru_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-ka")) {
	    i++;
	    all_cluster_file = argv[i];
	    continue;
	}
	if (!strcmp(argv[i], "-kp")) {
	    i++;
	    per_cluster_file = argv[i];
	    continue;
	}
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
		 "-l   <label-path-fmt>\n"
		 "-s   <sruns-file>\n"
		 "-f   <fruns-file>\n"
		 "-vs  <verbose-sites-file>\n"
		 "-cs  <compact-sites-file>\n"
		 "-u   <units-file-base>\n"
		 "-vr  <verbose-report-path-fmt>\n"
		 "-cr  <compact-report-path-fmt>\n"
		 "-e   <experiment-name>\n"
		 "-d   <program-src-dir>\n"
		 "-c   <confidence>\n"
		 "-t   <trace-file>\n"
		 "-pf  <preds-txt-full-file>\n"
		 "-pa  <preds-txt-abbr-file>\n"
		 "-pb  <preds-hdr-file>\n"
		 "-r   <result-summary-file>\n"
		 "-obs <obs-file>\n"
		 "-tru <tru-file>\n"
		 "-ka  <all-cluster-file>\n"
		 "-kp  <per-cluster-file>");
	    exit(0);
	}
	printf("Illegal option: %s\n", argv[i]);
	exit(1);
    }
}

void gen_views(const char bindir[], char* preds_file, int prefix)
{
    shell("awk '{ if ($1~/B/) print $0 }' < %s > B.txt", preds_file);
    shell("awk '{ if ($1~/R/) print $0 }' < %s > R.txt", preds_file);
    shell("awk '{ if ($1~/S/) print $0 }' < %s > S.txt", preds_file);
    shell("cp %s preds.txt", preds_file);

    shell("%s/%s -r %s -p %d", bindir, GEN_VIEWS, result_summary_file, prefix);
}

int main(int argc, char** argv)
{
    const char * const bindir = dirname(argv[0]);
    const char * const objdir = bindir;
    const char * const incdir = bindir;
    process_cmdline(argc, argv);

    if (do_process_labels) {
	if (sruns_file || fruns_file) {
	    puts("When you specify -do-process-labels, you must not specify -s or -f");
	    exit(1);
	}
	if (num_runs == -1 || !label_path_fmt) {
	    puts("When you specify -do-process-labels, you must also specify -n and -l");
	    exit(1);
	}
	puts("Processing run labels ...");
	sruns_file = DEFAULT_SRUNS_FILE;
	fruns_file = DEFAULT_FRUNS_FILE;
	shell("%s/%s -n %d -l %s -s %s -f %s",
	      bindir, PROCESS_RUN_LABELS, num_runs, label_path_fmt, sruns_file, fruns_file);
    }

    if (do_map_sites) {
	if (compact_sites_file || units_file_base)  {
	    puts("When you specify -do-map-sites, you must not specify -cs or -u");
	    exit(1);
	}
	if (!verbose_sites_file) {
	    puts("When you specify -do-map-sites, you must also specify -vs");
	    exit(1);
	}
	puts("Mapping sites ...");
	compact_sites_file = DEFAULT_COMPACT_SITES_FILE;
	units_file_base = DEFAULT_UNITS_FILE_BASE;
	shell("cat %s | "
	      "sed 's/[a-zA-Z_][a-zA-Z_0-9]*\\$//g' | "
	      "sed 's/\\([0-9][0-9]*\\)LL/\\1/g' | "
	      "sed 's/\\([0-9][0-9]*\\)ULL/\\1/g' | "
	      "%s/%s -cs %s -u %s.cc",
	      verbose_sites_file, bindir, MAP_SITES, compact_sites_file, units_file_base);
	shell("%s -I%s -c %s.cc", compiler, incdir, units_file_base);
    }

    if (do_convert_reports) {
	if (!sruns_file || !fruns_file || !verbose_report_path_fmt || !compact_report_path_fmt || !units_file_base) {
	    puts("When you specify -do-convert-reports, you must also specify -s, -f, -vr, -cr, and -u");
	    exit(1);
	}
	puts("Converting reports ...");
	shell("%s %s/convert_reports.o %s/classify_runs.o %s.o -o %s",
	      linker, objdir, objdir, units_file_base, CONVERT_REPORTS);
	shell("%s -s %s -f %s -vr %s -cr %s",
	      CONVERT_REPORTS, sruns_file, fruns_file, verbose_report_path_fmt, compact_report_path_fmt);
    }

    if (do_compute_results) {
	if (preds_full_file || preds_abbr_file || preds_file_base || result_summary_file) {
	    puts("When you specify -do-compute-results, you must not specify -pf, -pa, -pb, or -r");
	    exit(1);
	}
	if (!sruns_file || !fruns_file || !compact_sites_file || !compact_report_path_fmt || !units_file_base) {
	    puts("When you specify -do-compute-results, you must also specify -s, -f, -cs, -cr, and -u");
	    exit(1);
	}
	puts("Computing results ...");
	preds_full_file = DEFAULT_PREDS_FULL_FILE;
	preds_abbr_file = DEFAULT_PREDS_ABBR_FILE;
	preds_file_base  = DEFAULT_PREDS_FILE_BASE;
	result_summary_file = DEFAULT_RESULT_SUMMARY_FILE;
	shell("%s %s/compute_results.o %s/classify_runs.o %s/scaffold.o %s.o -o %s",
	      linker, objdir, objdir, objdir, units_file_base, COMPUTE_RESULTS);
	shell("%s -e %s -d %s -s %s -f %s -c %s -cs %s -cr %s %s %s -pf %s -pa %s -ps %s.cc -r %s",
	      COMPUTE_RESULTS, experiment_name, program_src_dir,
	      sruns_file, fruns_file, confidence,
	      compact_sites_file, compact_report_path_fmt,
	      ((trace_file) ? "-t" : ""), ((trace_file) ? trace_file : ""),
	      preds_full_file, preds_abbr_file, preds_file_base, result_summary_file);
	shell("%s -I%s -c %s.cc", compiler, incdir, preds_file_base);
    }

    if (do_compute_obs_tru) {
	if (obs_file || tru_file) {
	    puts("When you specify -do-compute-obs-tru, you must not specify -obs or -tru");
	    exit(1);
	}
	if (!sruns_file || !fruns_file || !preds_abbr_file || !compact_report_path_fmt || !units_file_base) {
	    puts("When you specify -do-compute-obs-tru, you must also specify -s, -f, -pa, -cr, and -u");
	    exit(1);
	}
	puts("Computing obs and tru ...");
	obs_file = DEFAULT_OBS_FILE;
	tru_file = DEFAULT_TRU_FILE;
	shell("%s %s/compute_obs_tru.o %s/classify_runs.o %s/scaffold.o %s.o -o %s",
	      linker, objdir, objdir, objdir, units_file_base, COMPUTE_OBS_TRU);
	shell("%s -s %s -f %s -pa %s -cr %s -obs %s -tru %s",
	      COMPUTE_OBS_TRU, sruns_file, fruns_file, preds_abbr_file,
	      compact_report_path_fmt, obs_file, tru_file);
    }

    if (do_print_results_1) {
	if (!result_summary_file || !preds_full_file) {
	    puts("When you specify -do-print-results-1, you must also specify -r and -pf");
	    exit(1);
	}
	puts("Pretty-printing results-1 ...");
	gen_views(bindir, preds_full_file, 0);
    }

    if (do_print_results_n) {
	if (!result_summary_file || !preds_file_base || !all_cluster_file || !per_cluster_file) {
	    puts("When you specify -do-print-results-n, you must also specify -r, -pb, -ka, and -kp");
	    exit(1);
	}
	puts("Pretty-printing results-n ...");
	shell("%s %s/gen_preds_file.o %s.o -o %s", linker, objdir, preds_file_base, GEN_PREDS_FILE);
	FILE* all_fp = fopen(all_cluster_file, "r");
	assert(all_fp);
	while (1) {
	    int x, y;
	    fscanf(all_fp, "%d %d", &x, &y);
	    if (feof(all_fp))
		break;
	    char file[1000];
	    sprintf(file, per_cluster_file, x);
	    shell("%s < %s > preds.%d.txt", GEN_PREDS_FILE, file, x);
	    sprintf(file, "preds.%d.txt", x);
	    gen_views(bindir, file, x);
	}
	fclose(all_fp);
    }

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
