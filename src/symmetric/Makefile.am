AM_CPPFLAGS = -W -Wall -Werror -Wformat=2

noinst_LIBRARIES = libsymmetric.a

libsymmetric_a_SOURCES =			\
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
	main.cc

../libanalyze.a: force
	$(MAKE) -C $(@D) $(@F)

force:
.PHONY: force
