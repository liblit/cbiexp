SUBDIRS = command-line

includes = -I command-line

include $(top_srcdir)/ocaml.mk.in
ocamlflags += -warn-error x

amplify_lexers =				\
	implLexer.mll				\
	sitesLexer.mll				\
	passOneLexer.mll

amplify_others =				\
	predicate.ml				\
	schemes.ml				\
	predicateTranslator.ml			\
	siteInfoAccumulator.ml			\
	implicationAccumulator.ml		\
	implications.ml				\
	runResultsAccumulator.ml		\
	reconstruct.ml				\
	logger.ml				\
	runner.ml

amplify_objects =				\
	predicate.$(cmo)			\
	schemes.$(cmo)				\
	predicateTranslator.$(cmo)		\
	siteInfoAccumulator.$(cmo)		\
	sitesLexer.$(cmo)			\
	implicationAccumulator.$(cmo)		\
	implLexer.$(cmo)			\
	implications.$(cmo)			\
	runResultsAccumulator.$(cmo)		\
	passOneLexer.$(cmo)			\
	reconstruct.$(cmo)			\
	logger.$(cmo)				\
	runner.$(cmo)

logreader_lexers = lineCountLexer.mll		\
	logImplLexer.mll    

logreader_others = predicate.ml			\
	implicationAccumulator.ml		\
	logReader.ml  

logreader_objects = predicate.$(cmo)		\
	implicationAccumulator.$(cmo)		\
	lineCountLexer.$(cmo)			\
	logImplLexer.$(cmo)			\
	logReader.$(cmo)

amplify_truth_probs_lexers = predsLexer.mll	\
	sitesLexer.mll				\
	implLexer.mll

amplify_truth_probs_others = predicate.ml	\
	schemes.ml				\
	logger.ml				\
	implicationAccumulator.ml		\
	siteInfoAccumulator.ml			\
	predicateTranslator.ml			\
	implications.ml				\
	predicateAccumulator.ml			\
	runResultsAccumulator.ml		\
	passOneLexer.ml				\
	amplifyTruthProbs.ml

amplify_truth_probs_objects = predicate.$(cmo)	\
	schemes.$(cmo)				\
	logger.$(cmo)				\
	implicationAccumulator.$(cmo)		\
	implLexer.$(cmo)			\
	siteInfoAccumulator.$(cmo)		\
	sitesLexer.$(cmo)			\
	predicateTranslator.$(cmo)		\
	implications.$(cmo)			\
	predicateAccumulator.$(cmo)		\
	predsLexer.$(cmo)			\
	runResultsAccumulator.$(cmo)		\
	passOneLexer.$(cmo)			\
	amplifyTruthProbs.$(cmo)
		

objects = $(amplify_objects) 			\
	$(logreader_objects)			\
	$(amplify_truth_probs_objects)

mlis = $(amplify_others:=.mli) 			\
	$(logreader_others:=.mli)     		\  
	$(amplify_truth_probs_others:=.mli)

depends = $(objects:.$(cmo)=.do) $(mlis:.mli=.di)

libs = command-line/commandLine.$(cma)

all-local: amplify logreader amplify-truth-probs

amplify: $(amplify_objects) $(libs)
	$(compiler) -o amplify $(libs) $(amplify_objects)

logreader: $(logreader_objects) $(libs)
	$(compiler) -o logreader $(libs) $(logreader_objects)

amplify-truth-probs: $(amplify_truth_probs_objects) $(libs)
	$(compiler) -o amplify-truth-probs $(libs) $(amplify_truth_probs_objects)

-include $(depends)
