texs := $(wildcard *.tex *.sty *.bib)
epss := $(wildcard *.eps)
pdfs := $(epss:.eps=.pdf)

all: master.pdf master.ps

master.pdf: %.pdf: $(texs) $(pdfs)
	pdflatex $*
	thumbpdf $*
	pdflatex $*

master.ps: %.ps: %.dvi
	dvips -o$@ -Pgeneric $*

master.dvi: %.dvi: $(texs) $(epss)
	latex $*
	bibtex $*
	latex $*
	latex $*

$(pdfs): %.pdf: %.eps
	epstopdf --outfile=$@ $<
