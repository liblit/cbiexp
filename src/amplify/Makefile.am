SUBDIRS = command-line

implication_lexer =	\
	implLexer.mll

implication_others =	\
	predicate.ml	\
	implicationAccumulator.ml	\
	implications.ml	\
	makeImplications.ml

implication_objects = 	\
	predicate.cmo	\
	implicationAccumulator.cmo	\
	implLexer.cmo	\
	implications.cmo	\
	makeImplications.cmo

amplify_lexers =					\
  implLexer.mll
	passOneLexer.mll			\
	passTwoLexer.mll

amplify_others =					\
	predicate.ml				\
	implicationAccumulator.ml	\
	implications.ml	\
	predicateAccumulator.ml			\
	predicates.ml				\
	reconstruct.ml				\
	runner.cmo

amplify_objects =					\
	predicate.cmo				\
	implicationAccumulator.cmo	\
  implLexer.cmo \
	implications.cmo	\
	predicateAccumulator.cmo		\
	predicates.cmo				\
	passOneLexer.cmo			\
	passTwoLexer.cmo			\
	reconstruct.cmo				\
	runner.cmo

objects = $(amplify_objects) $(implication_objects)

mlis = $(amplify_others:=.mli) $(implication_others:=.mli) 

depends = $(objects:.cmo=.do) $(mlis:.mli=.di)

libs = command-line/commandLine.cma

OCAMLC=ocamlc 
OCAMLOPT=ocamlopt 
OCAMLDEP=ocamldep 
OCAMLFLAGS=
OCAMLOPTFLAGS=
OCAMLLEX=ocamllex

includes = -I command-line

all-local: amplify makeImplications

amplify: $(amplify_objects) $(libs)
	$(OCAMLC) $(includes) $(OCAMLFLAGS) -o amplify $(libs) $(amplify_objects)

makeImplications: $(implication_objects)
	$(OCAMLC) $(includes) $(OCAMLFLAGS) -o makeImplications $(implication_objects)

# Common rules 
SUFFIXES: .ml .mli .cmo .cmi .cmx .di .do

.ml.cmo: 
	$(OCAMLC) $(includes) $(OCAMLFLAGS) -c $< 

.mli.cmi: 
	$(OCAMLC) $(includes) $(OCAMLFLAGS) -c $< 

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
	$(OCAMLDEP) $(includes) $^ >$@

.ml.do:
	$(OCAMLDEP) $(includes) $^ >$@

-include $(depends)
