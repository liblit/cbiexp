SUBDIRS = corrective-ranking Score symmetric

AM_CPPFLAGS = -W -Wall -Werror -Wformat=2 $(BOOST_CPPFLAGS)

EXTRA_DIST =					\
	summary.css				\
	summary.dtd				\
	summary.xsl

noinst_PROGRAMS =				\
	decimate-sparse				\
	map_sites				\
	process_run_labels

noinst_SCRIPTS =				\
	map_sites.sed

noinst_LIBRARIES = libanalyze.a liblate.a

all-local: analysis-rules.mk


map_sites_SOURCES = map_sites.cc
map_sites_LDADD = libanalyze.a

process_run_labels_SOURCES = process_run_labels.cc
process_run_labels_LDADD = libanalyze.a

decimate_sparse_SOURCES = decimate-sparse.ll
decimate_sparse_INCLUDES = $(GSL_CFLAGS)
decimate_sparse_LDADD = libanalyze.a $(GSL_LIBS)


########################################################################


libanalyze_a_SOURCES =				\
	AutoFD.cc				\
	AutoFD.h				\
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
	IndexedPredInfo.cc			\
	IndexedPredInfo.h			\
	MapSites.cc				\
	MapSites.h				\
	Nonconst_PredStats.cc			\
	Nonconst_PredStats.h			\
	NumRuns.cc				\
	NumRuns.h				\
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
	RunsDirectory.cc			\
	RunsDirectory.h				\
	RunsSplit.cc				\
	RunsSplit.h				\
	SiteCoords.cc				\
	SiteCoords.h				\
	SourceDirectory.cc			\
	SourceDirectory.h			\
	Stylesheet.cc				\
	Stylesheet.h				\
	TimestampReport.cc			\
	TimestampReport.h			\
	TsReportReader.cc			\
	TsReportReader.h			\
	UtilLogReg.cc				\
	UtilLogReg.h				\
	Verbose.cc				\
	Verbose.h				\
	ViewPrinter.cc				\
	ViewPrinter.h				\
	XMLTemplate.h				\
	XMLTemplate.cc				\
	classify_runs.cc			\
	classify_runs.h				\
	fopen.cc				\
	fopen.h					\
	shell.cc				\
	shell.h					\
	sorts.h					\
	split_runs.cc				\
	split_runs.h				\
	utils.cc				\
	utils.h


########################################################################
#
#  special handling for "late" programs
#
#  Some programs depend on data extracted from a particular suite of
#  runs.  We can compile them ahead of time, but linking is postponed.
#  The "analysis-rules.mk" makefile fragment takes care of preparing
#  the extra data objects and linking them with these
#  previously-compiled objects to produce complete programs.
#
#  The actual library archive is not used.  This is simply a hack to
#  trick automake into adding the appropriate build rules for the
#  constituent object files.
#

if HAVE_GSL_CDF
chi2test = chi2test.cc
endif

liblate_a_SOURCES =				\
	$(chi2test)				\
	build_tsgraph.cc			\
	calc_truthprobs.cc			\
	collect_parmstats.cc			\
	compute_obs_tru.cc			\
	compute_results.cc			\
	compute_nonconst_preds.cc		\
	compute_pred_scores.cc			\
	convert_reports.ll			\
	est_hyperparms.cc			\
	extract_timestamps.ll			\
	find_tsSCC.cc				\
	gather_prior_dist.cc			\
	gather_prior_truths.cc			\
	gen_preds_file.cc			\
	gen_summary.cc				\
	gen_views.cc				\
	get_tp_weights.cc			\
	get_ts_ordering.cc			\
	logreg.cc				\
	max-observed.cc				\
	mult_WX.cc				\
	partial_order.cc			\
	preds.h					\
	project.cc				\
	sanity_check.cc				\
	sites.h					\
	train_utillog.cc			\
	units.h					\
	xmlify-results.cc
