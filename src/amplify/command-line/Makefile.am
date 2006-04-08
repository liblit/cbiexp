mls = \
	commandLine	\
	runsDirectory	\
	implicationsReport	\
	inputReport	\
	logReport	\
	numRuns	\
	outputReport

mlis = $(mls:=.mli)
objects = $(mls:=.cmo)

depends = $(objects:.cmo=.do) $(mlis:.mli=.di)  

OCAMLC=ocamlc 
OCAMLOPT=ocamlopt 
OCAMLDEP=ocamldep 
OCAMLFLAGS=
OCAMLOPTFLAGS=

all-local: commandLine.cma

commandLine.cma: $(objects)
	$(OCAMLC) -a -o $@ $^
	
# Common rules 
SUFFIXES: .ml .mli .cmo .cmi .cmx .di .do

.ml.cmo: 
	$(OCAMLC) $(OCAMLFLAGS) -c $< 

.mli.cmi: 
	$(OCAMLC) $(OCAMLFLAGS) -c $< 

.ml.cmx: 
	$(OCAMLOPT) $(OCAMLOPTFLAGS) -c $< 

.mll.ml:
	$(OCAMLLEX) $<

clean-local: 
	rm -f *.d[io]
	rm -f *.cm[ioxa]

.mli.di:
	$(OCAMLDEP) $^ >$@

.ml.do:
	$(OCAMLDEP) $^ >$@

-include $(depends)
