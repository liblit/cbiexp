# @configure-input@

analysisdir ?= ../analysis
tooldir := $(root)/src/plsa
MATLAB = matlab -nodisplay
time := /usr/bin/time

numbugaspects ?= $(error numbugaspects not set)
numaspects ?= $(error numaspects not set)

all: expclean run
	$(MAKE) loglikelihood.txt

ifdef $(nonormalize)
runsinfo.mat: $(analysisdir)/runsinfo.mat
	cp -f $< $@ 
else
runsinfo.mat: $(analysisdir)/runsinfo.mat 
	$(MATLAB) -r "path('$(tooldir)', path); normalizeDocuments('$<', '$@'); quit"
endif
CLEANFILES += runsinfo.mat

run: runsinfo.mat
	$(time) $(tooldir)/run.py $(if $(maxiterations),--maxiterations $(maxiterations)) $(if $(minlikelihoodchange),--minlikelihoodchange $(minlikelihoodchange)) $(if $(numrestarts),--restarts $(numrestarts)) $(tooldir) $< $(numaspects) $(numbugaspects)
	touch $@
CLEANFILES += run

loglikelihood.txt: run
	$(time) $(tooldir)/rank.py $(foreach restart,$(dir $(wildcard */loglikelihood.txt)),-r $(restart)) 
CLEANFILES += loglikelihood.txt

# Cleans some files so that the experiment can be restarted from the same data 
# For a whole new plsa experiment, call sliver/plsaAnalyze.py
.PHONY : expclean 
expclean:
	-rm loglikelihood.txt run
	-rm -Rf best
