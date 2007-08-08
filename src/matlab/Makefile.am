AM_CPPFLAGS = -W -Wall -Werror -Wformat=2 $(BOOST_CPPFLAGS)

noinst_PROGRAMS =				\
	make-classify-vectors			\
	make-counts-matrix			\
	validate

make_classify_vectors_SOURCES = make_classify_vectors.cc
make_classify_vectors_LDADD = ../libanalyze.a -ldl

make_counts_matrix_SOURCES = make_counts_matrix.cc
make_counts_matrix_LDADD = ../libanalyze.a -ldl

validate: process.xml 
	xmllint --valid --noout process.xml
.PHONY: validate

force:
.PHONY: force
