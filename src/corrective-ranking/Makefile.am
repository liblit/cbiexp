AM_CPPFLAGS = -W -Wall -Werror -Wformat

noinst_LIBRARIES =				\
	libcorrective.a

noinst_PROGRAMS =				\
	adaptive-sampled			\
	approximate-complete			\
	exact-complete

libcorrective_a_SOURCES =			\
	Outcome.cc				\
	Outcome.h				\
	Predicates.cc				\
	Predicates.h				\
	Predicate.cc				\
	Predicate.h				\
	RunSet.cc				\
	RunSet.h				\
	RunSuite.cc				\
	RunSuite.h				\
	allFailures.cc				\
	allFailures.h				\
	main.cc					\
	main.h					\
	zoom.cc					\
	zoom.h

exact_complete_SOURCES = exact-complete.cc
exact_complete_LDADD = libcorrective.a ../libanalyze.a

approximate_complete_SOURCES = approximate-complete.cc
approximate_complete_LDADD = libcorrective.a ../libanalyze.a

adaptive_sampled_SOURCES = adaptive-sampled.cc
adaptive_sampled_LDADD = libcorrective.a ../libanalyze.a


../libanalyze.a: force
	$(MAKE) -C $(@D) $(@F)

force:
.PHONY: force
