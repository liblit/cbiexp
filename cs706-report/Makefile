MAIN      = report
SRCS      = $(wildcard *.tex *.bib *.sty *.cls)
PS        = $(MAIN).ps
PDF       = $(MAIN).pdf
DVI       = $(MAIN).dvi
BBL       = $(MAIN).bbl

siemens := print_tokens2 replace schedule schedule2 tcas tot_info
pycharts := effort pruning top-pred
others := maxsuccess
figs := $(siemens) $(pycharts) $(others)
epss := $(figs:%=charts/%.eps)
pdfs := $(figs:%=charts/%.pdf)

latex := latex -interaction=batchmode -file-line-error
pdflatex := pdf$(latex)

all: $(PDF) $(PS)

$(PDF): %.pdf: $(SRCS) $(BBL) $(pdfs)
	$(pdflatex) $*
	$(pdflatex) $*
	thumbpdf $*
	$(pdflatex) $*

$(BBL): %.bbl: $(SRCS) $(epss)
	$(latex) $*
	bibtex $*
	rm $*.dvi

$(DVI): %.dvi: $(SRCS) $(BBL) $(epss)
	$(latex) $*
	$(latex) $*

$(PS): %.ps: %.dvi $(epss)
	dvips -o $@ -P generic $<

%.pdf: %.eps
	epstopdf --outfile=$@ $<

$(pycharts:%=charts/%.eps): charts/%.eps: charts/%.py charts/common.py data/data.txt
	python $< $@

$(pycharts:%=charts/%.pdf): charts/%.pdf: charts/%.py charts/common.py data/data.txt
	python $< $@

clean:
	rm -f $(PS) $(PDF) $(DVI) $(BBL) $(pdfs) $(pycharts:%=charts/%.eps)

.DELETE_ON_ERROR:
