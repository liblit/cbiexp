AM_CPPFLAGS = -W -Wall -Werror

noinst_PROGRAMS =				\
	analyze_runs				\
	format					\
	gen_views				\
	map_sites				\
	process_run_labels

analyze_runs_SOURCES =				\
	analyze_runs.cc				\
	compiler.cc				\
	compiler.h				\
	shell.cc				\
	shell.h

format_SOURCES = format.cc

gen_views_SOURCES =				\
	gen_views.cc				\
	def.h					\
	shell.cc				\
	shell.h

map_sites_SOURCES = map_sites.cc

process_run_labels_SOURCES = process_run_labels.cc


########################################################################


noinst_LIBRARIES = libanalyze.a

libanalyze_a_SOURCES =				\
	classify_runs.cc			\
	classify_runs.h				\
	scaffold.cc				\
	scaffold.h				\
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

noinst_HEADERS =				\
	      preds.h				\
	      sites.h				\
	      units.h

late_programs =					\
	compute_obs_tru				\
	compute_results				\
	convert_reports				\
	gen_preds_file

late_objects = $(late_programs:=.o)
late_sources = $(late_programs:=.cc)
TAGS_FILES = $(late_sources)
EXTRA_DIST = $(late_sources)

all-local: $(late_objects)

$(late_objects): %.o: %.cc
