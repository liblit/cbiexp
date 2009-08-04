# @configure-input@

tooldir := $(root)/src/evaluateplsa

# analysis directory
analysisdir ?= ../../analysis

# where the original data, processed for plsa analysis can be found
datadir ?= ..

# where the results of the plsa analysis can be found
resultsdir ?= ../aspredicates

MATLAB = matlab -nodisplay
time := /usr/bin/time

all: RandIndex.txt

runsinfo.mat: $(datadir)/runsinfo.mat
	cp $< $@
CLEANFILES += runsinfo.mat

results.mat: $(resultsdir)/results.mat
	cp $< $@
CLEANFILES += results.mat

votes.mat: results.mat
	$(time) $(MATLAB) -r "path('$(tooldir)', path); generateVotes('$<', '$@'); quit()"

RandIndex.txt: votes.mat runsinfo.mat
	$(time) $(MATLAB) -r "path('$(tooldir)', path); calculateRandIndex('votes.mat', 'runsinfo.mat', '$@'); quit();"
CLEANFILES += RandIndex.txt

.PHONY: clean
clean:
	rm -Rf $(CLEANFILES)
