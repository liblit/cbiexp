name := moss
schemes := all branches returns scalar-pairs

view_flags := --stylesheet=moss-view.xsl
projected_view_flags := --stylesheet=projected-moss-view.xsl
corrected_view_flags := --stylesheet=corrected-moss-view.xsl

web_extras :=					\
	corrected-moss-view.xsl			\
	histograms.dtd				\
	histograms.xml				\
	moss-view-exports.xsl			\
	moss-view.xsl				\
	projected-moss-view.xsl

include ../../src/analysis-rules.mk

histograms.xml: %.xml: % run-bugs.txt tru.txt
	./$< >$@
	$(MAKE) $*.dtd
	xmllint --valid --noout $@
clean:: ; rm -f histograms.xml

run-bugs.txt: %.txt: f.runs %
	$(time) ./$* >$@
clean:: ; rm -f run-bugs.txt
