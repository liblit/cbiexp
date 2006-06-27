mls = \
	commandLine	\
	runsDirectory	\
	multiReports 	\
	perRunReport	\
	analysisReport	\
	flag		\	
	counts		\
	logging		\
	predRanking	\
	sitesReport 			\
	implicationsReport	\
	inputReport	\
	logReport	\
	numRuns	\
	outputReport \
	statsReport	\
	predRankReport

mlis = $(mls:=.mli)
objects = $(mls:=.$(cmo))

depends = $(objects:.$(cmo)=.do) $(mlis:.mli=.di)  

all-local: commandLine.$(cma)

commandLine.$(cma): $(objects)
	$(archive)
	
include $(top_srcdir)/ocaml.mk.in
-include $(depends)
