MAIN      = report
SRCS      = *.tex 
PS        = $(MAIN).ps
PDF       = $(MAIN).pdf
DVI       = $(MAIN).dvi
TARG      = $(MAIN).done

all: $(TARG) ps pdf

$(TARG): $(SRCS) bib
	latex $(MAIN)
	latex $(MAIN)
	touch $(TARG)

bib: 
	bibtex -min-crossrefs=100 $(MAIN)

ps: $(DVI)
	dvips -o $(PS) $(MAIN)

pdf: $(PS) $(DVI)
	ps2pdf14 $(PS)

fig: fig.gnu fig.data fig2.data
	gnuplot fig.gnu

clean:
	rm -f $(PS) $(PDF) $(DVI) $(MAIN).bbl $(TARG)