name := rhythmbox
schemes := all branches returns scalar-pairs
source-strip-prefixes := ../../src/ ../../../src/ ../../../../src/
training-runs := 1000
report-series := inverse-100

-include ../../site-rules.mk
-include site-rules.mk

include ../../src/analysis-rules.mk

histograms.xml: %.xml: % run-bugs.txt tru.txt
	./$< >$@
	$(MAKE) $*.dtd
	xmllint --valid --noout $@
clean:: ; rm -f histograms.xml

run-bugs.txt: %.txt: f.runs %
	$(time) ./$* $(datadir) >$@
clean:: ; rm -f run-bugs.txt
