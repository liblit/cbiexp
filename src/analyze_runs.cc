#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define PROCESS_RUN_LABELS "/moa/sc3/mhn/bin/process-run-labels"
#define MAP_SITES          "/moa/sc3/mhn/bin/map-sites"
#define CONVERT_REPORTS    "./convert-reports"
#define COMPUTE_RESULTS    "./compute-results"
#define COMPUTE_OBS_TRU    "./compute-obs-tru"
#define GEN_VIEWS          "/moa/sc3/mhn/bin/gen-views"
#define GEN_PREDS_FILE     "./gen-preds-file"

#define DEFAULT_SRUNS_FILE "s.runs"
#define DEFAULT_FRUNS_FILE "f.runs"
#define DEFAULT_COMPACT_SITES_FILE "sites.txt"
#define DEFAULT_UNITS_HDR_FILE "units.h"
#define DEFAULT_EXPERIMENT_NAME "dummy"
#define DEFAULT_PROGRAM_SRC_DIR "."
#define DEFAULT_CONFIDENCE "95"
#define DEFAULT_PREDS_FULL_FILE "preds.full.txt"
#define DEFAULT_PREDS_ABBR_FILE "preds.abbr.txt"
#define DEFAULT_PREDS_HDR_FILE "preds.h"
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
char* units_hdr_file = NULL;

char* verbose_report_path_fmt = NULL;
char* compact_report_path_fmt = NULL;

char* experiment_name = DEFAULT_EXPERIMENT_NAME;
char* program_src_dir = DEFAULT_PROGRAM_SRC_DIR;
char* confidence = DEFAULT_CONFIDENCE;
char* trace_file = NULL;
char* preds_full_file = NULL;
char* preds_abbr_file = NULL;
char* preds_hdr_file = NULL;
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
	    units_hdr_file = argv[i];
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
	if (!strcmp(argv[i], "-ph")) {
	    i++;
	    preds_hdr_file = argv[i];
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
	    printf("Usage: analyze-runs <options>\n"
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
		   "-u   <units-hdr-file>\n"
		   "-vr  <verbose-report-path-fmt>\n"
		   "-cr  <compact-report-path-fmt>\n"
		   "-e   <experiment-name>\n"
		   "-d   <program-src-dir>\n"
		   "-c   <confidence>\n"
		   "-t   <trace-file>\n"
		   "-pf  <preds-txt-full-file>\n"
		   "-pa  <preds-txt-abbr-file>\n"
		   "-ph  <preds-hdr-file>\n"
		   "-r   <result-summary-file>\n"
		   "-obs <obs-file>\n"
		   "-tru <tru-file>\n"
		   "-ka  <all-cluster-file>\n"
		   "-kp  <per-cluster-file>\n");
	    exit(0);
	}
	printf("Illegal option: %s\n", argv[i]);
	exit(1);
    }
}

char cmd[1000];

void gen_views(char* preds_file, int prefix)
{
    sprintf(cmd, "awk '{ if ($1~/B/) print $0 }' < %s > B.txt", preds_file);
    system (cmd);
    sprintf(cmd, "awk '{ if ($1~/R/) print $0 }' < %s > R.txt", preds_file);
    system (cmd);
    sprintf(cmd, "awk '{ if ($1~/S/) print $0 }' < %s > S.txt", preds_file);
    system (cmd);
    sprintf(cmd, "cp %s preds.txt", preds_file);
    system (cmd);

    sprintf(cmd, "%s -r %s -p %d", GEN_VIEWS, result_summary_file, prefix);
    system (cmd);
}

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);

    if (do_process_labels) {
	if (sruns_file || fruns_file) {
	    printf("When you specify -do-process-labels, you must not specify -s or -f\n");
	    exit(1);
	}
	if (num_runs == -1 || !label_path_fmt) {
	    printf("When you specify -do-process-labels, you must also specify -n and -l\n");
	    exit(1);
	}
	printf("Processing run labels ...\n");
	sruns_file = DEFAULT_SRUNS_FILE;
	fruns_file = DEFAULT_FRUNS_FILE;
	sprintf(cmd, "%s -n %d -l %s -s %s -f %s",
		PROCESS_RUN_LABELS, num_runs, label_path_fmt, sruns_file, fruns_file);
	system (cmd);
    }

    if (do_map_sites) {
	if (compact_sites_file || units_hdr_file)  {
	    printf("When you specify -do-map-sites, you must not specify -cs or -u\n");
	    exit(1);
	}
	if (!verbose_sites_file) {
	    printf("When you specify -do-map-sites, you must also specify -vs\n");
	    exit(1);
	}
	printf("Mapping sites ...\n");
	compact_sites_file = DEFAULT_COMPACT_SITES_FILE;
	units_hdr_file = DEFAULT_UNITS_HDR_FILE;
	sprintf(cmd, "cat %s | "
		     "sed 's/[a-zA-Z_][a-zA-Z_0-9]*\\$//g' | "
		     "sed 's/\\([0-9][0-9]*\\)LL/\\1/g' | "
		     "sed 's/\\([0-9][0-9]*\\)ULL/\\1/g' | "
		     "%s -cs %s -u %s",
		     verbose_sites_file, MAP_SITES, compact_sites_file, units_hdr_file);
	system (cmd);
    }

    if (do_convert_reports) {
	if (!sruns_file || !fruns_file || !verbose_report_path_fmt || !compact_report_path_fmt || !units_hdr_file) {
	    printf("When you specify -do-convert-reports, you must also specify -s, -f, -vr, -cr, and -u\n");
	    exit(1);
	}
	printf("Converting reports ...\n");
	sprintf(cmd, "g++ -O3 /moa/sc3/mhn/src/convert_reports.cc /moa/sc3/mhn/src/classify_runs.cc -I/moa/sc3/mhn/src -include %s -o %s",
		     units_hdr_file, CONVERT_REPORTS);
	system (cmd);
	sprintf(cmd, "%s -s %s -f %s -vr %s -cr %s",
		     CONVERT_REPORTS, sruns_file, fruns_file, verbose_report_path_fmt, compact_report_path_fmt);
	system (cmd);
    }

    if (do_compute_results) {
	if (preds_full_file || preds_abbr_file || preds_hdr_file || result_summary_file) {
	    printf("When you specify -do-compute-results, you must not specify -pf, -pa, -ph, or -r\n");
	    exit(1);
	}
	if (!sruns_file || !fruns_file || !compact_sites_file || !compact_report_path_fmt || !units_hdr_file) {
	    printf("When you specify -do-compute-results, you must also specify -s, -f, -cs, -cr, and -u\n");
	    exit(1);
	}
	printf("Computing results ...\n");
	preds_full_file = DEFAULT_PREDS_FULL_FILE;
	preds_abbr_file = DEFAULT_PREDS_ABBR_FILE;
	preds_hdr_file  = DEFAULT_PREDS_HDR_FILE;
	result_summary_file = DEFAULT_RESULT_SUMMARY_FILE;
	sprintf(cmd, "g++ -O3 /moa/sc3/mhn/src/compute_results.cc /moa/sc3/mhn/src/classify_runs.cc /moa/sc3/mhn/src/scaffold.cc -I/moa/sc3/mhn/src/ -include %s -o %s",
		     units_hdr_file, COMPUTE_RESULTS);
	system (cmd);
	sprintf(cmd, "%s -e %s -d %s -s %s -f %s -c %s -cs %s -cr %s %s %s -pf %s -pa %s -ph %s -r %s",
		     COMPUTE_RESULTS, experiment_name, program_src_dir,
		     sruns_file, fruns_file, confidence,
		     compact_sites_file, compact_report_path_fmt,
		     ((trace_file) ? "-t" : ""), ((trace_file) ? trace_file : ""),
		     preds_full_file, preds_abbr_file, preds_hdr_file, result_summary_file);
	system (cmd);

    }

    if (do_compute_obs_tru) {
	if (obs_file || tru_file) {
	    printf("When you specify -do-compute-obs-tru, you must not specify -obs or -tru\n");
	    exit(1);
	}
	if (!sruns_file || !fruns_file || !preds_abbr_file || !compact_report_path_fmt || !units_hdr_file) {
	    printf("When you specify -do-compute-obs-tru, you must also specify -s, -f, -pa, -cr, and -u\n");
	    exit(1);
	}
	printf("Computing obs and tru ...\n");
	obs_file = DEFAULT_OBS_FILE;
	tru_file = DEFAULT_TRU_FILE;
	sprintf(cmd, "g++ -O3 /moa/sc3/mhn/src/compute_obs_tru.cc /moa/sc3/mhn/src/classify_runs.cc /moa/sc3/mhn/src/scaffold.cc -I/moa/sc3/mhn/src/ -include %s -o %s",
		     units_hdr_file, COMPUTE_OBS_TRU);
	system (cmd);
	sprintf(cmd, "%s -s %s -f %s -pa %s -cr %s -obs %s -tru %s",
		     COMPUTE_OBS_TRU, sruns_file, fruns_file, preds_abbr_file,
		     compact_report_path_fmt, obs_file, tru_file);
	system (cmd);
    }

    if (do_print_results_1) {
	if (!result_summary_file || !preds_full_file) {
	    printf("When you specify -do-print-results-1, you must also specify -r and -pf\n");
	    exit(1);
	}
	printf("Pretty-printing results-1 ...\n");
	gen_views(preds_full_file, 0);
    }

    if (do_print_results_n) {
	if (!result_summary_file || !preds_hdr_file || !all_cluster_file || !per_cluster_file) {
	    printf("When you specify -do-print-results-n, you must also specify -r, -ph, -ka, and -kp\n");
	    exit(1);
	}
	printf("Pretty-printing results-n ...\n");
	sprintf(cmd, "g++ -O3 /moa/sc3/mhn/src/gen_preds_file.cc -include %s -o %s", preds_hdr_file, GEN_PREDS_FILE);
	system (cmd);
	FILE* all_fp = fopen(all_cluster_file, "r");
	assert(all_fp);
	while (1) {
	    int x, y;
	    fscanf(all_fp, "%d %d", &x, &y);
	    if (feof(all_fp))
		break;
	    char file[1000];
	    sprintf(file, per_cluster_file, x);
	    sprintf(cmd, "%s < %s > preds.%d.txt", GEN_PREDS_FILE, file, x);
	    system (cmd);
	    sprintf(file, "preds.%d.txt", x);
	    gen_views(file, x);
	}
	fclose(all_fp);
    }

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
