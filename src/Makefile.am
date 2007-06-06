SUBDIRS = amplify corrective-ranking lsa mutual-information never-reached Score symmetric

AM_CPPFLAGS = -W -Wall -Werror -Wformat=2 $(BOOST_CPPFLAGS)

if HAVE_GSL_CDF
chi2test = chi2test
endif

noinst_PROGRAMS =				\
	build_tsgraph				\
	calc_sober				\
	calc_truthprobs				\
	$(chi2test)				\
	cluster_runs				\
	collect_parmstats			\
	compute_nonconst_preds			\
	compute-obs-tru				\
	compute_pred_scores			\
	compute-results				\
	convert-reports				\
	decimate-sparse				\
	est_parms				\
	extract-timestamps			\
	find_tsSCC				\
	gather_prior_dist			\
	gather_prior_truths			\
	gen-summary				\
	gen-views				\
	get_tp_weights				\
	get_truth_values			\
	get_ts_ordering				\
	logreg					\
	max-observed				\
	oracle					\
	partial_order				\
        process_run_flags                       \
	process_run_labels			\
	project					\
	sampling-plans/uniform			\
	sanity_check				\
	sparsity-metrics			\
	train_utillog				\
	view_clusters				\
	xmlify-results

noinst_SCRIPTS =				\
	decimate-all				\
	htmlify-sources				\
	map-sites				\
	static-site-info			\
	top-rho

noinst_LIBRARIES = libanalyze.a

all-local: analysis-rules.mk


build_tsgraph_SOURCES = build_tsgraph.cc
build_tsgraph_LDADD = libanalyze.a -ldl
build_tsgraph_CPPFLAGS = $(AM_CPPFLAGS) -Wno-uninitialized

calc_sober_SOURCES = calc_sober.cc
calc_sober_LDADD = libanalyze.a $(GSL_LIBS) -ldl

calc_truthprobs_SOURCES = calc_truthprobs.cc
calc_truthprobs_LDADD = libanalyze.a -ldl

chi2test_SOURCES = chi2test.cc
chi2test_LDADD = libanalyze.a $(GSL_LIBS)

cluster_runs_SOURCES = cluster_runs.cc
cluster_runs_LDADD = libanalyze.a $(GSL_LIBS) -ldl

collect_parmstats_SOURCES = collect_parmstats.cc
collect_parmstats_LDADD = libanalyze.a -ldl

compute_obs_tru_SOURCES = compute_obs_tru.cc
compute_obs_tru_LDADD = libanalyze.a -ldl

compute_nonconst_preds_SOURCES = compute_nonconst_preds.cc
compute_nonconst_preds_LDADD = libanalyze.a -ldl $(GSL_LIBS)

compute_pred_scores_SOURCES = compute_pred_scores.cc
compute_pred_scores_LDADD = libanalyze.a

compute_results_SOURCES = compute_results.cc
compute_results_LDADD = libanalyze.a -ldl

convert_reports_SOURCES = convert_reports.ll
convert_reports_LDADD = libanalyze.a -ldl $(LEXLIB)

decimate_sparse_SOURCES = decimate-sparse.ll
decimate_sparse_INCLUDES = $(GSL_CFLAGS)
decimate_sparse_LDADD = libanalyze.a $(GSL_LIBS)

est_parms_SOURCES = est_parms.cc
est_parms_LDADD = libanalyze.a $(GSL_LIBS)

extract_timestamps_SOURCES = extract_timestamps.ll
extract_timestamps_LDADD = libanalyze.a -ldl $(LEXLIB)

find_tsSCC_SOURCES = find_tsSCC.cc
find_tsSCC_LDADD = libanalyze.a -ldl
find_tsSCC_CPPFLAGS = $(AM_CPPFLAGS) -Wno-uninitialized

gather_prior_dist_SOURCES = gather_prior_dist.cc
gather_prior_dist_LDADD = libanalyze.a -ldl

gather_prior_truths_SOURCES = gather_prior_truths.cc
gather_prior_truths_LDADD = libanalyze.a -ldl

gen_summary_SOURCES = gen_summary.cc
gen_summary_LDADD = libanalyze.a -ldl

gen_views_SOURCES = gen_views.cc
gen_views_LDADD = Score/libScore.a libanalyze.a -ldl

get_tp_weights_SOURCES = get_tp_weights.cc
get_tp_weights_LDADD = libanalyze.a -ldl

get_truth_values_SOURCES = get_truth_values.cc
get_truth_values_LDADD = libanalyze.a -ldl

get_ts_ordering_SOURCES = get_ts_ordering.cc
get_ts_ordering_LDADD = libanalyze.a -ldl

logreg_SOURCES = logreg.cc
logreg_LDADD = libanalyze.a -ldl

max_observed_SOURCES = max-observed.cc
max_observed_LDADD = libanalyze.a -ldl

oracle_SOURCES = oracle.cc
oracle_LDADD = libanalyze.a -ldl

partial_order_SOURCES = partial_order.cc
partial_order_LDADD = libanalyze.a -ldl
partial_order_CPPFLAGS = $(AM_CPPFLAGS) -Wno-uninitialized

process_run_flags_SOURCES = process_run_flags.cc
process_run_flags_LDADD = libanalyze.a

process_run_labels_SOURCES = process_run_labels.cc
process_run_labels_LDADD = libanalyze.a

project_SOURCES = project.cc
project_LDADD = Score/libScore.a libanalyze.a -ldl

sampling_plans_uniform_SOURCES = sampling-plans/uniform.cc
sampling_plans_uniform_LDADD = libanalyze.a -ldl

sanity_check_SOURCES = sanity_check.cc
sanity_check_LDADD = libanalyze.a -ldl
sanity_check_CPPFLAGS = $(AM_CPPFLAGS) -Wno-uninitialized

sparsity_metrics_SOURCES = sparsity-metrics.cc
sparsity_metrics_LDADD = libanalyze.a -ldl
sparsity_metrics_CPPFLAGS = $(AM_CPPFLAGS) -Wno-uninitialized

train_utillog_SOURCES = train_utillog.cc
train_utillog_LDADD = libanalyze.a -ldl $(GSL_LIBS)

view_clusters_SOURCES = view_clusters.cc
view_clusters_LDADD = libanalyze.a

xmlify_results_SOURCES = xmlify-results.cc
xmlify_results_LDADD = libanalyze.a -ldl


########################################################################


libanalyze_a_SOURCES =				\
	AmplifyReport.cc			\
	AmplifyReport.h				\
	AutoFD.cc				\
	AutoFD.h				\
	Bugs.cc					\
	Bugs.h					\
	Candidate.cc				\
	Candidate.h				\
	ClassifyRuns.cc				\
	ClassifyRuns.h				\
	CompactReport.cc			\
	CompactReport.h				\
	Confidence.cc				\
	Confidence.h				\
	Dilute.cc				\
	Dilute.h				\
	DynamicLibrary.h			\
	DynamicLibrary-dl.cc			\
	DynamicLibrary-dl.h			\
	IndexedPredInfo.cc			\
	IndexedPredInfo.h			\
	MappedArray.h				\
	Matrix.h				\
	Nonconst_PredStats.cc			\
	Nonconst_PredStats.h			\
	NumRuns.cc				\
	NumRuns.h				\
	OutcomeRunSets.cc			\
	OutcomeRunSets.h			\
	PredCoords.cc				\
	PredCoords.h				\
	PredSet.cc				\
	PredSet.h				\
	PredStats.cc				\
	PredStats.h				\
	Progress/Bounded.cc			\
	Progress/Bounded.h			\
	Progress/Unbounded.cc			\
	Progress/Unbounded.h			\
	Progress/tty.cc				\
	Progress/tty.h				\
	RawReport.cc				\
	RawReport.h				\
	ReportReader.cc				\
	ReportReader.h				\
	Rho.cc					\
	Rho.h					\
	RunBugs.cc				\
	RunBugs.h				\
	RunsDirectory.cc			\
	RunsDirectory.h				\
	RunSet.cc				\
	RunSet.h				\
	RunsSplit.cc				\
	RunsSplit.h				\
	SetVector.cc				\
	SetVector.h				\
	SiteCoords.cc				\
	SiteCoords.h				\
	SourceDirectory.cc			\
	SourceDirectory.h			\
	Stylesheet.cc				\
	Stylesheet.h				\
	TimestampReport.cc			\
	TimestampReport.h			\
	AdjWeightsOptions.cc			\
	AdjWeightsOptions.h			\
	TsReportReader.cc			\
	TsReportReader.h			\
	StaticSiteInfo.cc			\
	StaticSiteInfo.h			\
	UtilLogReg.cc				\
	UtilLogReg.h				\
	Verbose.cc				\
	Verbose.h				\
	ViewPrinter.cc				\
	ViewPrinter.h				\
	XMLTemplate.cc				\
	XMLTemplate.h				\
	arguments.h				\
	classify_runs.cc			\
	classify_runs.h				\
	fopen.cc				\
	fopen.h					\
	preds.h					\
	site_t.h				\
	sorts.h					\
	split_runs.cc				\
	split_runs.h				\
	termination.h				\
	timestamp.h				\
	unit_t.h				\
	utils.cc				\
	utils.h


########################################################################


EXTRA_DIST =					\
	$(noinst_SCRIPTS)			\
	bug-o-meter.css				\
	bug-o-meter.dtd				\
	bug-o-meter.js				\
	bug-o-meter.xsl				\
	corrected-view.dtd			\
	corrected-view.xsl			\
	link-cell.css				\
	logo.css				\
	logo.xsl				\
	logreg.dtd				\
	logreg.xsl				\
	operand.dtd				\
	operands.xsl				\
	predictor-info.dtd			\
	pred-scores.css				\
	pred-scores.dtd				\
	pred-scores.xsl				\
	projected-view.dtd			\
	projected-view.xsl			\
	projections.dtd				\
	projections.xml				\
	rho.dtd					\
	rho.xsl					\
	schemes.dtd				\
	schemes.xml				\
	scores.css				\
	scores.dtd				\
	scores.xsl				\
	sorts.dtd				\
	sorts.xml				\
	static-site-info.dtd			\
	summary.css				\
	summary.dtd				\
	summary.xsl				\
	symmetric.css				\
	symmetric.dtd				\
	symmetric.xsl				\
	view.css				\
	view.dtd				\
	view.xsl
