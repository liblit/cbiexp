SUBDIRS = command-line

includes = -I command-line

include $(top_srcdir)/ocaml.mk.in

amplify_lexers =					\
  implLexer.mll		\
	passOneLexer.mll			\
	passTwoLexer.mll

amplify_others =					\
	predicate.ml				\
	implicationAccumulator.ml	\
	implications.ml	\
	predicateAccumulator.ml			\
	predicates.ml				\
	reconstruct.ml				\
	logger.ml	\
	runner.ml

amplify_objects =					\
	predicate.$(cmo)				\
	implicationAccumulator.$(cmo)	\
  implLexer.$(cmo) \
	implications.$(cmo)	\
	predicateAccumulator.$(cmo)		\
	predicates.$(cmo)				\
	passOneLexer.$(cmo)			\
	passTwoLexer.$(cmo)			\
	reconstruct.$(cmo)				\
	logger.$(cmo)	\
	runner.$(cmo)

objects = $(amplify_objects) 

mlis = $(amplify_others:=.mli) 

depends = $(objects:.$(cmo)=.do) $(mlis:.mli=.di)

libs = command-line/commandLine.$(cma)

all-local: amplify 

amplify: $(amplify_objects) $(libs) 
	$(compiler) -o amplify $(libs) $(amplify_objects)

-include $(depends)
