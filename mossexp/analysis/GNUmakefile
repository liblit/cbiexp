root := ../..
experiment := ..
datadir := $(experiment)/data
corrdir := $(root)/correlations
tooldir := $(root)/src
sites := $(wildcard $(experiment)/share/*.sites)

time := /usr/bin/time

sparsebase := fobs sobs ftr str
sparse := $(sparsebase:=.) $(sparsebase:=.ir) $(sparsebase:=.jc) $(sparsebase:=.meta)

schemes := all branches returns scalar-pairs
sorts := lb is fs nf hl hs
projections := none circular linear
views := $(foreach scheme, $(schemes), $(foreach sort, $(sorts), $(foreach projection, $(projections), $(scheme)_$(sort)_$(projection).xml)))

topRho := $(foreach sort, hl hs, $(foreach proj, circular linear, top-rho_$(sort)_$(proj).xml))
web := logo.css logo.xsl sorts.dtd sorts.xml summary.css summary.dtd summary.xml summary.xsl $(views) projected-view.dtd projected-view.xsl view.css view.dtd view.xsl projections.xml projections.dtd predictor-info.xml predictor-info.dtd moss-view.xsl projected-moss-view.xsl histograms.xml histograms.dtd $(topRho) rho.dtd rho.xsl
publish := $(HOME)/www/project/moss-new

all: $(web)
.PHONY: all

publish: $(web:%=$(publish)/%)
.PHONY: publish

$(publish)/%: %
	[ -d $(@D) ] || mkdir $(@D)
	cp $< $@

projected-view.dtd projected-view.xsl projections.xml projections.dtd logo.css logo.xsl summary.css summary.dtd sorts.dtd sorts.xml predictor-info.dtd:
	ln -s $(tooldir)/$@ .
clean:: ; rm -f projected-view.dtd projected-view.xsl projections.dtd projections.xml summary.css summary.dtd sorts.dtd sorts.xml logo.css logo.xsl

predictor-info.xml: xmlify-results preds.txt
	./$<
	$(MAKE) $(@:.xml=.dtd)
	xmllint --valid --noout $@

xmlify-results: %: $(tooldir)/%.o sites.o units.o $(tooldir)/libanalyze.a
	$(LINK.cc) $^ -o $@

$(filter-out %_none.xml, $(views)): project preds.txt rho.bin
	$(time) ./$< --stylesheet=projected-moss-view.xsl
	$(MAKE) projected-view.dtd
	xmllint --valid --noout $(filter-out %_none.xml, $(views))
clean:: ; rm -f $(filter-out %_none.xml, $(views))

project: $(tooldir)/project.o sites.o units.o $(tooldir)/libanalyze.a $(tooldir)/Score/libScore.a
	$(LINK.cc) $^ -o $@
clean:: ; rm -f project

$(topRho): top-rho_%: top-rho all_% rho.bin
	./$<
	xmllint --valid --noout $(topRho)

rho.bin: calculate.m $(sparse)
	echo "fwrite(fopen('rho.bin','w'), rho, 'double');" | $(time) matlab -nodisplay -nojvm -r calculate
clean:: ; rm -rf rho.bin mats

calculate.m: $(corrdir)/genMscript.pl $(sparsebase:=.meta)
	$(time) ./$< .
clean:: ; rm -f calculate.m

$(sparse): $(corrdir)/mhn2sparsemat.pl f.runs s.runs obs.txt tru.txt
	$(time) ./$< . .
clean:: ; rm -f $(sparse)

obs.txt tru.txt: $(tooldir)/compute_obs_tru.o $(datadir)/0/reports.sparse preds.txt s.runs f.runs sites.o units.o
	$(time) $(tooldir)/analyze_runs --do=compute-obs-tru --runs-directory=$(datadir)
clean:: ; rm -f obs.txt tru.txt compute-obs-tru

$(filter %_none.xml, $(views)): $(tooldir)/gen_views.o sites.o units.o preds.txt
	$(time) $(tooldir)/analyze_runs --do=print-results-1 --stylesheet=moss-view.xsl
	xmllint --valid --noout $(filter %_none.xml, $(views))
clean:: ; rm -f $(filter %_none.xml, $(views)) view.css view.dtd view.xsl gen-views

summary.xml: $(tooldir)/gen_summary.o preds.txt s.runs f.runs sites.o units.o
	$(time) $(tooldir)/analyze_runs --do=print-summary --source-directory=../src
	[ -r $@ ]
clean:: ; rm -f summary.xml summary.css summary.dtd summary.xsl gen-summary

preds.txt: $(tooldir)/compute_results.o sites.o units.o s.runs f.runs $(datadir)/0/reports.sparse
	$(time) $(tooldir)/analyze_runs --do=compute-results --runs-directory=$(datadir)
clean:: ; rm -f preds.txt compute-results

$(datadir)/0/reports.sparse: $(tooldir)/convert_reports.o $(datadir)/stamp-labels s.runs f.runs units.o
	$(time) $(tooldir)/analyze_runs --do=convert-reports --runs-directory=$(datadir)
clean:: ; rm -f convert-reports

s.runs f.runs: $(tooldir)/process_run_labels.o $(datadir)/stamp-labels
	$(time) $(tooldir)/analyze_runs --do=process-labels --runs-directory=$(datadir)
clean:: ; rm -f s.runs f.runs

histograms.xml: %.xml: % run-bug.txt tru.txt
	./$< >$@
	$(MAKE) $*.dtd
	xmllint --valid --noout $@

run-bug.txt: %.txt: $(datadir)/stamp-labels %
	$(time) ./$* `cat $<` >$@
clean:: ; rm -f run-bug.txt

sites.o units.o: $(tooldir)/map_sites.o $(sites)
	$(time) $(tooldir)/analyze_runs --do=map-sites $(sites:%=--sites-text=%)
clean:: ; rm -f sites.o units.o sites.cc units.cc

$(tooldir)/%: force
	$(MAKE) -C $(@D) $(@F)

.PHONY: clean

force:
.PHONY: force

.DELETE_ON_ERROR:
