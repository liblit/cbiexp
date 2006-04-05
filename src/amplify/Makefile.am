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
	passOneLexer.mll			\
	passTwoLexer.mll

amplify_others =					\
	predicate.ml				\
	impls.ml				\
	predicateAccumulator.ml			\
	predicates.ml				\
	reconstruct.ml				\
	runner.cmo

amplify_objects =					\
	predicate.cmo				\
	predicateAccumulator.cmo		\
	impls.cmo		\
	predicates.cmo				\
	passOneLexer.cmo			\
	passTwoLexer.cmo			\
	reconstruct.cmo				\
	runner.cmo

objects = $(amplify_objects) $(implication_objects)

mlis = $(amplify_others:=.mli) $(implication_others:=.mli) 

depends = $(objects:.cmo=.do) $(mlis:.mli=.di)

OCAMLC=ocamlc 
OCAMLOPT=ocamlopt 
OCAMLDEP=ocamldep 
OCAMLFLAGS=
OCAMLOPTFLAGS=
OCAMLLEX=ocamllex

all-local: amplify makeImplications

amplify: $(amplify_objects)
	$(OCAMLC) $(OCAMLFLAGS) -o amplify $(amplify_objects)

makeImplications: $(implication_objects)
	$(OCAMLC) $(OCAMLFLAGS) -o makeImplications $(implication_objects)

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

-include $(depends)
