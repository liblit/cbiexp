SUBDIRS = corrective-ranking Score

AM_CPPFLAGS = -W -Wall -Werror -Wformat

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
decimate_sprase_INCLUDES = $(CSL_CFLAGS)
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
	SiteCoords.cc				\
	SiteCoords.h				\
	SourceDirectory.cc			\
	SourceDirectory.h			\
	Stylesheet.cc				\
	Stylesheet.h				\
	Verbose.cc				\
	Verbose.h				\
	ViewPrinter.cc				\
	ViewPrinter.h				\
	classify_runs.cc			\
	classify_runs.h				\
	fopen.cc				\
	fopen.h					\
	shell.cc				\
	shell.h					\
	sorts.h					\
	utils.cc				\
	utils.h					\
	split_runs.cc				\
	split_runs.h				\
	RunsSplit.cc				\
	RunsSplit.h				\
	Nonconst_PredStats.cc			\
	Nonconst_PredStats.h			\
	UtilLogReg.cc				\
	UtilLogReg.h


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

liblate_a_SOURCES =				\
	compute_obs_tru.cc			\
	compute_results.cc			\
	compute_nonconst_preds.cc		\
	train_utillog.cc			\
	convert_reports.ll			\
	gather_prior_dist.cc			\
	gen_preds_file.cc			\
	gen_summary.cc				\
	gen_views.cc				\
	logreg.cc				\
	max-observed.cc				\
	preds.h					\
	project.cc				\
	sites.h					\
	units.h					\
	xmlify-results.cc
