lexers =					\
	implLexer.mll				\
	passOneLexer.mll			\
	passTwoLexer.mll

others =					\
	implicationAccumulator.ml		\
	predicate.ml				\
	implications.ml				\
	predicateAccumulator.ml			\
	predicates.ml				\
	reconstruct.ml				\
	runner.ml

objects =					\
	predicate.cmo				\
	implicationAccumulator.cmo		\
	implications.cmo			\
	predicateAccumulator.cmo		\
	predicates.cmo				\
	implLexer.cmo				\
	passOneLexer.cmo			\
	passTwoLexer.cmo			\
	reconstruct.cmo				\
	runner.cmo

mls = $(lexers:.mll=.ml) $(others)
mlis = $(others:=.mli)

depends = $(objects:.cmo=.do) $(mlis:.mli=.di)

OCAMLC=ocamlc 
OCAMLOPT=ocamlopt 
OCAMLDEP=ocamldep 
OCAMLFLAGS=
OCAMLOPTFLAGS=
OCAMLLEX=ocamllex

all-local: amplify

amplify: $(objects)
	$(OCAMLC) $(OCAMLFLAGS) -o amplify $(objects)

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
	rm -f *.cm[iox]
	rm -f implLexer.ml
	rm -f passOneLexer.ml
	rm -f passTwoLexer.ml 

.mli.di:
	$(OCAMLDEP) $^ >$@

.ml.do:
	$(OCAMLDEP) $^ >$@

#include .lexdepend
-include $(depends)
