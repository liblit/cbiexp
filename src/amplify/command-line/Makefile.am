mls = \
	logging	\
	commandLine	\
	runsDirectory	\
	multiReports 	\
	sitesReport 			\
	implicationsReport	\
	inputReport	\
	logReport	\
	numRuns	\
	outputReport \
	statsReport

mlis = $(mls:=.mli)
objects = $(mls:=.$(cmo))

depends = $(objects:.$(cmo)=.do) $(mlis:.mli=.di)  

all-local: commandLine.$(cma)

commandLine.$(cma): $(objects)
	$(archive)
	
include $(top_srcdir)/ocaml.mk.in
-include $(depends)
