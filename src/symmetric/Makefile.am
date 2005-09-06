AM_CPPFLAGS = -W -Wall -Werror -Wformat=2 $(BOOST_CPPFLAGS)

noinst_PROGRAMS = symmetric

symmetric_SOURCES =				\
	Both.h					\
	Boths.cc				\
	Boths.h					\
	Candidates.cc				\
	Candidates.h				\
	Counts.cc				\
	Counts.h				\
	FailureReader.cc			\
	FailureReader.h				\
	Predicate.cc				\
	Predicate.h				\
	Reader.cc				\
	Reader.h				\
	RunSet.cc				\
	RunSet.h				\
	SuccessReader.cc			\
	SuccessReader.h				\
	allFailures.cc				\
	allFailures.h				\
	main.cc

symmetric_LDADD = ../libanalyze.a -ldl

../libanalyze.a: force
	$(MAKE) -C $(@D) $(@F)

force:
.PHONY: force
