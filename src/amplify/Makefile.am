noinst_PROGRAMS = amplify

amplify_SOURCES = implLexer.mll 
amplify_SOURCES += implicationAccumulator.ml
amplify_SOURCES += implicationAccumulator.mli
amplify_SOURCES += implications.ml
amplify_SOURCES += implications.mli
amplify_SOURCES += passOneLexer.mll
amplify_SOURCES += passTwoLexer.mll
amplify_SOURCES += predicate.ml
amplify_SOURCES += predicate.mli
amplify_SOURCES += predicateAccumulator.ml
amplify_SOURCES += predicateAccumulator.mli
amplify_SOURCES += predicates.ml
amplify_SOURCES += predicates.mli
amplify_SOURCES += reconstruct.ml
amplify_SOURCES += reconstruct.mli
amplify_SOURCES += runner.ml

OBJECTS = predicate.cmo 
OBJECTS += implLexer.cmo 
OBJECTS += passOneLexer.cmo 
OBJECTS += passTwoLexer.cmo 
OBJECTS += implicationAccumulator.cmo 
OBJECTS += predicateAccumulator.cmo 
OBJECTS += implications.cmo 
OBJECTS += predicates.cmo 
OBJECTS += reconstruct.cmo 
OBJECTS += runner.cmo 
	
OCAMLC=ocamlc 
OCAMLOPT=ocamlopt 
OCAMLDEP=ocamldep 
OCAMLFLAGS=
OCAMLOPTFLAGS=
OCAMLLEX=ocamllex

amplify$(EXEEXT): $(OBJECTS)
	$(OCAMLC) $(OCAMLFLAGS) -o amplify $(OBJECTS)

# Common rules 
SUFFIXES: .ml .mli .cmo .cmi .cmx 

.ml.cmo: 
	$(OCAMLC) $(OCAMLFLAGS) -c $< 

.mli.cmi: 
	$(OCAMLC) $(OCAMLFLAGS) -c $< 

.ml.cmx: 
	$(OCAMLOPT) $(OCAMLOPTFLAGS) -c $< 

.mll.ml:
	$(OCAMLLEX) $<

clean-local: 
	rm -f *.cm[iox]
	rm -f implLexer.ml
	rm -f passOneLexer.ml
	rm -f passTwoLexer.ml 

include $(srcdir)/.lexdepend
include $(srcdir)/.depend
