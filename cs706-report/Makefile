MAIN      = report
SRCS      = $(wildcard *.tex *.bib *.sty *.cls) charts/sampling.tex
PS        = $(MAIN).ps
PDF       = $(MAIN).pdf
DVI       = $(MAIN).dvi
BBL       = $(MAIN).bbl

siemens := print_tokens print_tokens2 replace schedule schedule2 tcas tot_info
sampling := $(siemens:%=sampling-%)
pycharts := effort-conj effort-disj pruning-conj pruning-disj top-pred
others := maxsuccess $(sampling)
figs := $(pycharts) $(others)
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

charts/pruning.eps charts/top-pred.eps: charts/%.eps: charts/%.py charts/common.py data/data.txt
	python $< $@

charts/pruning.pdf charts/top-pred.pdf: charts/%.pdf: charts/%.py charts/common.py data/data.txt
	python $< $@

charts/sampling.tex $(sampling:%=charts/%.eps) $(sampling:%=charts/%.pdf): charts/sampling.py charts/common.py data/data.txt
	python $<

charts/pruning-conj.eps charts/pruning-disj.eps charts/pruning-conj.pdf charts/pruning-disj.pdf: charts/pruning.py charts/common.py data/data.txt
	python $< $@ $(subst pruning-, , $(basename $(@F)))

charts/effort-conj.eps charts/effort-disj.eps charts/effort-conj.pdf charts/effort-disj.pdf: charts/effort.py charts/common.py data/data.txt
	python $< $@ $(subst effort-, , $(basename $(@F)))

clean:
	rm -f $(PS) $(PDF) $(DVI) $(BBL) $(MAIN).aux $(MAIN).out $(pdfs) $(pycharts:%=charts/%.eps)

.DELETE_ON_ERROR:
