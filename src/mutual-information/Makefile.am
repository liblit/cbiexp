AM_CPPFLAGS = -W -Wall -Werror -Wformat=2 $(BOOST_CPPFLAGS)

noinst_PROGRAMS =				\
	assign-runs-to-pred-sets		\
	calc-MI-oracle				\
	compute-MI-pred-clusters		\
	compute-pred-bug-counts			\
	compute-pred-bug-MI			\
	compute-pred-entropy			\
	compute-pred-pred-MI			\
	normalize-pred-pred-MI

assign_runs_to_pred_sets_SOURCES = assign_runs_to_pred_sets.cc
assign_runs_to_pred_sets_LDADD = ../libanalyze.a -ldl 

calc_MI_oracle_SOURCES = calc_MI_oracle.cc 
calc_MI_oracle_LDADD = ../libanalyze.a -ldl 

compute_MI_pred_clusters_SOURCES = compute_MI_pred_clusters.cc 
compute_MI_pred_clusters_LDADD = ../libanalyze.a -ldl 

compute_pred_bug_counts_SOURCES = compute_pred_bug_counts.cc 
compute_pred_bug_counts_LDADD = ../libanalyze.a -ldl 

compute_pred_bug_MI_SOURCES = compute_pred_bug_MI.cc 
compute_pred_bug_MI_LDADD = ../libanalyze.a -ldl 

compute_pred_entropy_SOURCES = compute_pred_entropy.cc 
compute_pred_entropy_LDADD = ../libanalyze.a -ldl 

compute_pred_pred_MI_SOURCES = compute_pred_pred_MI.cc
compute_pred_pred_MI_LDADD = ../libanalyze.a -ldl 

normalize_pred_pred_MI_SOURCES = normalize_pred_pred_MI.cc
normalize_pred_pred_MI_LDADD = ../libanalyze.a -ldl 

../libanalyze.a: force
	$(MAKE) -C $(@D) $(@F)

force:
.PHONY: force
