texs :=						\
	alg.tex					\
	bibnames.sty				\
	cacm1990.bib				\
	experiments.tex				\
	gcbib.bib				\
	icse02.bib				\
	icse03.bib				\
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
	setup.tex				\
	table.tex				\
	views/bc/rows.tex			\
	views/bug-o-meter.sty			\
	views/ccrypt/rows.tex			\
	views/exif/rows.tex			\
	views/rhythmbox/rows.tex		\
	views/view.sty

epss := predkept3a.eps predkept3b.eps
pdfs := $(epss:.eps=.pdf)

all: master.pdf master.ps

pubbase := $(shell echo $$HOME)/public/html/pldi-2005/pldi-2005
publish := $(pubbase).pdf $(pubbase).ps
publish: $(publish)
$(publish): $(pubbase).%: master.%
	cat $< >$@

master.pdf: %.pdf: %.dvi $(texs) $(pdfs)
	pdflatex $*
	thumbpdf $*
	pdflatex $*

master.ps: %.ps: %.dvi
	dvips -o$@ -Pgeneric $*

master.dvi: %.dvi: $(texs) $(epss)
	latex  $*
	bibtex $*
	latex  $*
	latex  $*

$(pdfs): %.pdf: %.eps
	epstopdf --outfile=$@ $<

moss-view/rows.tex moss-view/stats-by-%.tex: force
	$(MAKE) -C $(@D) $(@F)

views/%/rows.tex: force
	$(MAKE) -C $(@D) $(@F)

force:
