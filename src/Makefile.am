AM_CPPFLAGS = -W -Wall -Werror
AM_LFLAGS = -CFar -s

noinst_PROGRAMS =				\
	analyze_runs				\
	map_sites				\
	process_run_labels

noinst_SCRIPTS =				\
	map_sites.sed

noinst_LIBRARIES = libanalyze.a liblate.a


analyze_runs_SOURCES =				\
	analyze_runs.cc				\
	compiler.cc				\
	compiler.h				\
	shell.cc				\
	shell.h

map_sites_SOURCES = map_sites.cc

process_run_labels_SOURCES = process_run_labels.cc


########################################################################


libanalyze_a_SOURCES =				\
	classify_runs.cc			\
	classify_runs.h				\
	utils.cc				\
	utils.h


########################################################################
#
#  special handling for "late" programs
#
#  Some programs depend on data extracted from a particular suite of
#  runs.  We can compile them ahead of time, but linking is postponed.
#  The "analyze_runs" program takes care of preparing the extra data
#  objects and linking them with these previously-compiled objects to
#  produce complete programs.
#
#  The actual library archive is not used.  This is simply a hack to
#  trick automake into adding the appropriate build rules for the
#  constituent object files.
#

liblate_a_SOURCES =				\
	compute_obs_tru.cc			\
	compute_results.cc			\
	convert_reports.ll			\
	gen_preds_file.cc			\
	gen_views.cc				\
	preds.h					\
	sites.h					\
	units.h
