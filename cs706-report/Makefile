MAIN      = report
SRCS      = $(wildcard *.tex *.bib *.sty *.cls)
PS        = $(MAIN).ps
PDF       = $(MAIN).pdf
DVI       = $(MAIN).dvi
BBL       = $(MAIN).bbl
FIGS      = $(wildcard charts/*.eps)

latex := latex -interaction=batchmode -file-line-error
pdflatex := pdf$(latex)

all: $(PDF) $(PS)

$(PDF): %.pdf: $(SRCS) $(BBL) $(FIGS:.eps=.pdf)
	$(pdflatex) $*
	$(pdflatex) $*
	thumbpdf $*
	$(pdflatex) $*

$(BBL): %.bbl: $(SRCS) $(FIGS)
	$(latex) $*
	bibtex $*

$(DVI): %.dvi: $(SRCS) $(BBL) $(FIGS)
	$(latex) $*
	$(latex) $*

$(PS): %.ps: %.dvi $(FIGS)
	dvips -o $@ -P generic $<

$(FIGS:.eps=.pdf): %.pdf: %.eps
	epstopdf --outfile=$@ $<

clean:
	rm -f $(PS) $(PDF) $(DVI) $(BBL)

.DELETE_ON_ERROR:
