AM_CPPFLAGS = -W -Wall -Werror -Wformat=2 $(BOOST_CPPFLAGS)

noinst_PROGRAMS =				\
	make-binarized-matrix

make_binarized_matrix_SOURCES = make_binarized_matrix.cc
make_binarized_matrix_LDADD = ../libanalyze.a -ldl

force:
.PHONY: force
