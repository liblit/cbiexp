mls = \
	commandLine	\
	runsDirectory	\
	multiReports 	\
	perRunReport	\
	analysisReport	\
	flag		\	
	numRuns

mlis = $(mls:=.mli)
objects = $(mls:=.$(cmo))

depends = $(objects:.$(cmo)=.do) $(mlis:.mli=.di)  

all-local: commandLine.$(cma)

commandLine.$(cma): $(objects)
	$(archive)
	
include $(top_srcdir)/ocaml.mk.in
-include $(depends)
