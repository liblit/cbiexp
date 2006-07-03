name := moss
schemes := all branches returns scalar-pairs

view_flags := --stylesheet=moss-view.xsl
projected_view_flags := --stylesheet=projected-moss-view.xsl
corrected_view_flags := --stylesheet=corrected-moss-view.xsl
training-runs := 1000
report-serires := inverse-100

web_extras :=					\
	corrected-moss-view.xsl			\
	histograms.dtd				\
	histograms.xml				\
	histograms.xsl				\
	moss-view.xsl				\
	projected-moss-view.xsl

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
