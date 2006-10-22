AM_CPPFLAGS = -W -Wall -Werror -Wformat=2 $(BOOST_CPPFLAGS)

noinst_LIBRARIES =				\
	libmutualinformation.a

noinst_PROGRAMS =				\
	assign-run-sets-to-pred-sets		\
	assign-runs-to-pred-sets		\
	calc-MI-oracle				\
	compute-MI-pred-clusters		\
	compute-MI-run-clusters			\	
	compute-pred-bug-counts			\
	compute-pred-bug-MI			\
	compute-pred-entropy			\
	compute-pred-pred-MI			\
	compute-run-run-MI			\
	convert-csv				\
	normalize-pred-pred-MI			\
	normalize-run-run-MI

libmutualinformation_a_SOURCES =		\
	FailureUniverse.cc			\
	FailureUniverse.h			\
	MIUniverse.cc				\
	MIUniverse.h				\
	PredUniverse.cc				\
	PredUniverse.h

assign_run_sets_to_pred_sets_SOURCES = assign_run_sets_to_pred_sets.cc
assign_run_sets_to_pred_sets_LDADD = libmutualinformation.a ../libanalyze.a -ldl

assign_runs_to_pred_sets_SOURCES = assign_runs_to_pred_sets.cc
assign_runs_to_pred_sets_LDADD = libmutualinformation.a ../libanalyze.a -ldl

calc_MI_oracle_SOURCES = calc_MI_oracle.cc 
calc_MI_oracle_LDADD = libmutualinformation.a ../libanalyze.a -ldl 

compute_MI_pred_clusters_SOURCES = compute_MI_pred_clusters.cc 
compute_MI_pred_clusters_LDADD = libmutualinformation.a ../libanalyze.a -ldl 

compute_MI_run_clusters_SOURCES = compute_MI_run_clusters.cc 
compute_MI_run_clusters_LDADD = libmutualinformation.a ../libanalyze.a -ldl 

compute_pred_bug_counts_SOURCES = compute_pred_bug_counts.cc 
compute_pred_bug_counts_LDADD = libmutualinformation.a ../libanalyze.a -ldl 

compute_pred_bug_MI_SOURCES = compute_pred_bug_MI.cc 
compute_pred_bug_MI_LDADD = libmutualinformation.a ../libanalyze.a -ldl 

compute_pred_entropy_SOURCES = compute_pred_entropy.cc 
compute_pred_entropy_LDADD = libmutualinformation.a ../libanalyze.a -ldl 

compute_pred_pred_MI_SOURCES = compute_pred_pred_MI.cc
compute_pred_pred_MI_LDADD = libmutualinformation.a ../libanalyze.a -ldl 

compute_run_run_MI_SOURCES = compute_run_run_MI.cc
compute_run_run_MI_LDADD = libmutualinformation.a ../libanalyze.a -ldl 

convert_csv_SOURCES = convert_csv.cc
convert_csv_LDADD = libmutualinformation.a ../libanalyze.a -ldl 

normalize_pred_pred_MI_SOURCES = normalize_pred_pred_MI.cc
normalize_pred_pred_MI_LDADD = libmutualinformation.a ../libanalyze.a -ldl 

normalize_run_run_MI_SOURCES = normalize_run_run_MI.cc
normalize_run_run_MI_LDADD = libmutualinformation.a ../libanalyze.a -ldl 

../libanalyze.a: force
	$(MAKE) -C $(@D) $(@F)

force:
.PHONY: force
