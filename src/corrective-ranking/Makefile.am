AM_CPPFLAGS = -W -Wall -Werror -Wformat

noinst_LIBRARIES = libcorrective.a

noinst_PROGRAMS =				\
	approximate-complete			\
	exact-complete

libcorrective_a_SOURCES =			\
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
	main.h

exact_complete_SOURCES =			\
	exact-complete.cc

exact_complete_LDADD = libcorrective.a ../libanalyze.a

approximate_complete_SOURCES =			\
	CountingIterator.h			\
	approximate-complete.cc

approximate_complete_LDADD = libcorrective.a ../libanalyze.a

../libanalyze.a: force
	$(MAKE) -C $(@D) $(@F)

force:
.PHONY: force
