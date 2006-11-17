AM_CPPFLAGS = -W -Wall -Werror -Wformat=2 $(BOOST_CPPFLAGS)

noinst_PROGRAMS =				\
	make-binarized-matrix			\
	make-classify-vectors			\
	make-counts-matrix			\
	xmlify-preds

make_binarized_matrix_SOURCES = make_binarized_matrix.cc
make_binarized_matrix_LDADD = ../libanalyze.a -ldl

make_classify_vectors_SOURCES = make_classify_vectors.cc
make_classify_vectors_LDADD = ../libanalyze.a -ldl

make_counts_matrix_SOURCES = make_counts_matrix.cc
make_counts_matrix_LDADD = ../libanalyze.a -ldl

xmlify_preds_SOURCES = xmlify_preds.cc
xmlify_preds_LDADD = ../libanalyze.a -ldl

force:
.PHONY: force
