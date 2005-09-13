mexglx = readsp.mexglx

all-local: $(mexglx)

.c.mexglx:
	mex $<

CLEANFILES = $(mexglx)

EXTRA_DIST =					\
	genMscript.pl				\
	mhn2sparsemat.pl			\
	readsp.c
