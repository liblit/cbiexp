mls = \
	logging	\
	commandLine	\
	runsDirectory	\
	sitesReport 			\
	implicationsReport	\
	inputReport	\
	logReport	\
	numRuns	\
	outputReport

mlis = $(mls:=.mli)
objects = $(mls:=.$(cmo))

depends = $(objects:.$(cmo)=.do) $(mlis:.mli=.di)  

all-local: commandLine.$(cma)

commandLine.$(cma): $(objects)
	$(archive)
	
include $(top_srcdir)/ocaml.mk.in
-include $(depends)
