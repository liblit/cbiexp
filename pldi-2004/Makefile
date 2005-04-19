texs :=						\
	alg.tex					\
	bibnames.sty				\
	cacm1990.bib				\
	experiments.tex				\
	extensions.tex				\
	gcbib.bib				\
	icse02.bib				\
	icse03.bib				\
	mailto.sty				\
	master.tex				\
	misc.bib				\
	moss-view/rows.tex			\
	moss-view/stats-breakout.tex		\
	moss-view/stats-by-failure.tex		\
	moss-view/stats-by-harmonic.tex		\
	moss-view/stats-by-increase.tex		\
	moss-view/table.tex			\
	nips16.bib				\
	paste02.bib				\
	pldi03.bib				\
	pods.bib				\
	ramss.bib				\
	refs.bib				\
	revisited.tex				\
	runs.tex				\
	subtab.sty				\
	table.tex				\
	views/bc/rows.tex			\
	views/bug-o-meter.sty			\
	views/ccrypt/rows.tex			\
	views/exif/rows.tex			\
	views/rhythmbox/rows.tex		\
	views/view.sty

all: master.pdf master.ps

pubbase := $(shell echo $$HOME)/public/html/pldi-2005/pldi-2005
publish := $(pubbase).pdf $(pubbase).ps
publish: $(publish)
$(publish): $(pubbase).%: master.%
	cat $< >$@

master.pdf: %.pdf: %.dvi $(texs)
	pdflatex $*
	thumbpdf $*
	pdflatex $*

master.ps: %.ps: %.dvi
	dvips -o$@ -Pgeneric $*

master.dvi: %.dvi: %.bbl $(texs)
	latex  $*
	latex  $*

master.bbl: %.bbl: $(texs)
	latex $*
	bibtex $*

flat.tex: master.bbl $(texs)
	cat master.tex | sed 's/\\bibliography{\(.*\)}/\\input{master.bbl}/' | texexpand - >$@

moss-view/rows.tex moss-view/stats-by-%.tex: force
	$(MAKE) -C $(@D) $(@F)

views/%/rows.tex: force
	$(MAKE) -C $(@D) $(@F)

force:
