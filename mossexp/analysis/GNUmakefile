name := moss
schemes := all branches returns scalar-pairs
projected_view_flags := --stylesheet=projected-moss-view.xsl
view_flags := --stylesheet=moss-view.xsl
web_extras := moss-view.xsl projected-moss-view.xsl histograms.xml histograms.dtd

include ../../src/analysis-rules.mk

histograms.xml: %.xml: % run-bug.txt tru.txt
	./$< >$@
	$(MAKE) $*.dtd
	xmllint --valid --noout $@

run-bug.txt: %.txt: $(datadir)/stamp-labels %
	$(time) ./$* `cat $<` >$@
clean:: ; rm -f run-bug.txt
