SUBDIRS = command-line

includes = -I command-line

include $(top_srcdir)/ocaml.mk.in

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
	predicateAccumulator.ml			\
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
	predicateAccumulator.$(cmo)		\
	passOneLexer.$(cmo)			\
	reconstruct.$(cmo)			\
	logger.$(cmo)				\
	runner.$(cmo)

logreader_lexers = logLexer.mll

logreader_others = logReader.ml

logreader_objects =				\
	logLexer.$(cmo)				\
	logReader.$(cmo)

objects = $(amplify_objects) $(logreader_objects)

mlis = $(amplify_others:=.mli) $(logreader_other:=.mli)

depends = $(objects:.$(cmo)=.do) $(mlis:.mli=.di)

libs = command-line/commandLine.$(cma)

all-local: amplify logreader

amplify: $(amplify_objects) $(libs)
	$(compiler) -o amplify $(libs) $(amplify_objects)

logreader: $(logreader_objects) $(libs)
	$(compiler) -o logreader $(libs) $(logreader_objects)

-include $(depends)
