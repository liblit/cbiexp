AM_CPPFLAGS = -W -Wall -Werror -Wformat=2 $(BOOST_CPPFLAGS)

EXTRA_DIST =					\
	bugs2sparse.py				\
	convertBugsInfo.m			\
	convertData.m				\
	convertInfo.m				\
	convertPriorResults.m			\
	explode.m				\
	postProcess.m				\
	process.xml				\
	removeAllZeroRuns.m			\
	removeFractionSucceedingRuns.m		\
	toDAformat.m
	

noinst_PROGRAMS =				\
	make-classify-vectors			\
	make-counts-matrix

make_classify_vectors_SOURCES = make_classify_vectors.cc
make_classify_vectors_LDADD = ../libanalyze.a -ldl

make_counts_matrix_SOURCES = make_counts_matrix.cc
make_counts_matrix_LDADD = ../libanalyze.a -ldl

validate: process.xml
	xmllint --valid --noout $< 
.PHONY: validate

force:
.PHONY: force
