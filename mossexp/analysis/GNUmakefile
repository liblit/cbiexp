root := ../..
experiment := ..
datadir := $(experiment)/data
corrdir := $(root)/correlations
tooldir := $(root)/src
sites := $(wildcard $(experiment)/share/*.sites)

time := /usr/bin/time

sparsebase := fobs sobs ftr str
sparse := $(sparsebase:=.) $(sparsebase:=.ir) $(sparsebase:=.jc) $(sparsebase:=.meta)

sorts := lb is fs nf
schemes := all
schemes += branches
#schemes += g-object-unref
schemes += returns
schemes += scalar-pairs
views := $(foreach scheme, $(schemes), $(foreach sort, $(sorts), $(scheme)_$(sort).xml))

web := logo.css logo.xsl sorts.dtd sorts.xml summary.css summary.dtd summary.xml summary.xsl $(views) projected.xml projected-view.dtd projected-view.xsl view.css view.dtd view.xsl

all: $(web)
	xmllint --valid --noout *.xml
.PHONY: all

copy: $(web:%=$(HOME)/www/project/moss/harmonic/%)
.PHONY: copy

$(HOME)/www/project/moss/harmonic/%: %
	cp $< $@

projected-view.dtd projected-view.xsl:
	ln -s $(tooldir)/$@ .

projected.xml: project preds.txt rho.txt
	$(time) ./$< >$@
clean:: ; rm -f projected.xml

project: $(tooldir)/project.o sites.o units.o $(tooldir)/libanalyze.a $(tooldir)/Score/libScore.a
	$(time) $(LINK.cc) $^ -o $@
clean:: ; rm -f project

rho.txt: calculate.m $(sparse)
	echo "save 'rho.txt' rho -ascii;" | $(time) matlab -nodisplay -nojvm -r calculate
clean:: ; rm -rf rho.txt mats

calculate.m: $(corrdir)/genMscript.pl $(sparsebase:=.meta)
	$(time) ./$< .
clean:: ; rm -f calculate.m

$(sparse): $(corrdir)/mhn2sparsemat.pl f.runs s.runs obs.txt tru.txt
	$(time) ./$< . .
clean:: ; rm -f $(sparse)

obs.txt tru.txt: $(datadir)/stamp-labels $(datadir)/0/reports.new preds.txt s.runs f.runs sites.o units.o
	$(time) $(tooldir)/analyze_runs -do-compute-obs-tru -n `cat $<` -ss sites -us units -s s.runs -f f.runs -p preds.txt -cr $(datadir)/%d/reports.new
clean:: ; rm -f obs.txt tru.txt compute-obs-tru

$(views): $(tooldir)/gen_views.o sites.o units.o preds.txt
	$(time) $(tooldir)/analyze_runs -do-print-results-1 -r summary.xml -p preds.txt -ss sites -us units
clean:: ; rm -f $(views) view.css view.dtd view.xsl gen-views

summary.xml: $(tooldir)/gen_summary.o preds.txt s.runs f.runs sites.o units.o
	$(time) $(tooldir)/analyze_runs -do-print-summary -p preds.txt -s s.runs -f f.runs -r $@ -ss sites -us units -d ../src
clean:: ; rm -f summary.xml summary.css summary.dtd summary.xsl logo.css logo.xsl sorts.dtd sorts.xml gen-summary

preds.txt: $(tooldir)/compute_results.o sites.o units.o s.runs f.runs $(datadir)/0/reports.new
	$(time) $(tooldir)/analyze_runs -do-compute-results -ss sites -us units -s s.runs -f f.runs -cr $(datadir)/%d/reports.new
clean:: ; rm -f preds.txt compute-results

$(datadir)/0/reports.new: $(datadir)/stamp-labels s.runs f.runs units.o
	$(time) $(tooldir)/analyze_runs -do-convert-reports -vr $(datadir)/%d/reports -cr $(datadir)/%d/reports.new -s s.runs -f f.runs -us units
clean:: ; rm -f convert-reports

s.runs f.runs: $(datadir)/stamp-labels
	$(time) $(tooldir)/analyze_runs -do-process-labels -l $(datadir)/%d/label -n `cat $<`
clean:: ; rm -f s.runs f.runs

sites.o units.o: $(sites)
	$(time) $(tooldir)/analyze_runs -do-map-sites $(sites:%=-st %)
clean:: ; rm -f sites.o units.o sites.cc units.cc

$(tooldir)/%: force
	$(MAKE) -C $(@D) $(@F)

.PHONY: clean

force:
.PHONY: force

.DELETE_ON_ERROR:
