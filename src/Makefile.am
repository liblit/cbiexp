SUBDIRS = corrective-ranking evaluateplsa fromplsa matlab never-reached Score
if ENABLE_AMPLIFY
SUBDIRS += amplify
endif
if HAVE_CODESURFER
SUBDIRS += effort
endif

DIST_SUBDIRS = $(SUBDIRS) effort

AM_CPPFLAGS = -W -Wall -Werror -Wformat=2 -Wno-deprecated $(BOOST_CPPFLAGS)

noinst_PROGRAMS =				\
	bicluster				\
	calc_sober				\
	cluster_runs				\
	collect_parmstats			\
	compute_nonconst_preds			\
	compute-obs-tru				\
	est_parms				\
	gather_prior_dist			\
	gather_prior_truths			\
	gen-summary				\
	gen-views				\
	get_tp_weights				\
	logreg					\
	prefilter				\
	project					\
	train_utillog				\
	xmlify-results

noinst_SCRIPTS =				\
	top-rho

noinst_LIBRARIES = libanalyze.a

all-local: analysis-rules.mk


bicluster_SOURCES = bicluster.cc
bicluster_LDADD = libanalyze.a

calc_sober_SOURCES = calc_sober.cc
calc_sober_LDADD = libanalyze.a $(GSL_LIBS) -ldl

cluster_runs_SOURCES = cluster_runs.cc
cluster_runs_LDADD = libanalyze.a $(GSL_LIBS) -ldl

collect_parmstats_SOURCES = collect_parmstats.cc
collect_parmstats_LDADD = libanalyze.a -ldl

compute_obs_tru_SOURCES = compute_obs_tru.cc
compute_obs_tru_LDADD = libanalyze.a -ldl

compute_nonconst_preds_SOURCES = compute_nonconst_preds.cc
compute_nonconst_preds_LDADD = libanalyze.a -ldl $(GSL_LIBS)

est_parms_SOURCES = est_parms.cc
est_parms_LDADD = libanalyze.a $(GSL_LIBS)

gather_prior_dist_SOURCES = gather_prior_dist.cc
gather_prior_dist_LDADD = libanalyze.a -ldl

gather_prior_truths_SOURCES = gather_prior_truths.cc
gather_prior_truths_LDADD = libanalyze.a -ldl

gen_summary_SOURCES = gen_summary.cc
gen_summary_LDADD = libanalyze.a -ldl

gen_views_SOURCES = gen_views.cc
gen_views_LDADD = Score/libScore.a libanalyze.a corrective-ranking/libcorrective.a -ldl

get_tp_weights_SOURCES = get_tp_weights.cc
get_tp_weights_LDADD = libanalyze.a -ldl

logreg_SOURCES = logreg.cc
logreg_LDADD = libanalyze.a -ldl

prefilter_SOURCES = prefilter.cc
prefilter_LDADD = libanalyze.a -ldl

project_SOURCES = project.cc
project_LDADD = Score/libScore.a libanalyze.a -ldl

train_utillog_SOURCES = train_utillog.cc
train_utillog_LDADD = libanalyze.a -ldl $(GSL_LIBS)

xmlify_results_SOURCES = xmlify-results.cc
xmlify_results_LDADD = libanalyze.a -ldl


########################################################################


libanalyze_a_SOURCES =				\
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
	Counts.h				\
	Dilute.cc				\
	Dilute.h				\
	DiscreteDist.h				\
	DynamicLibrary.h			\
	DynamicLibrary-dl.cc			\
	DynamicLibrary-dl.h			\
	EstimateTPs.cc                          \
	EstimateTPs.h                           \
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
	SampleRatesFile.h			\
	SampleRatesFile.cc			\
	SetVector.cc				\
	SetVector.h				\
	SiteCoords.cc				\
	SiteCoords.h				\
	SourceDirectory.cc			\
	SourceDirectory.h			\
	Stylesheet.cc				\
	Stylesheet.h				\
	AdjWeightsOptions.cc			\
	AdjWeightsOptions.h			\
	StaticSiteInfo.cc			\
	StaticSiteInfo.h			\
	UtilLogReg.cc				\
	UtilLogReg.h				\
	Verbose.cc				\
	Verbose.h				\
	ViewPrinter.cc				\
	ViewPrinter.h				\
	arguments.h				\
	classify_runs.cc			\
	classify_runs.h				\
	combine.cc				\
	combine.h				\
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
	utils.h					\
	xml.cc					\
	xml.h


########################################################################


EXTRA_DIST =					\
	$(noinst_SCRIPTS)			\
	bug-o-meter.css				\
	bug-o-meter.dtd				\
	bug-o-meter.js				\
	bug-o-meter.xsl				\
	corrected-view.dtd			\
	corrected-view.xsl			\
	DecimateAll.pm				\
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
	summary.css				\
	summary.dtd				\
	summary.xsl				\
	view.css				\
	view.dtd				\
	view.xsl
