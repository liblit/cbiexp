ranking := all_hl_corrected-exact-complete.xml
postprocess := ../views/postprocess
dependencies := ../views/bug-o-meter.xsl ../../src/operands.xsl predictor-info.xml $(postprocess)
xsltproc := xsltproc --path '. ../views ../../src'

all: rows.tex stats-by-failure.tex stats-by-increase.tex stats-by-harmonic.tex

rows.tex: $(ranking) corrected-moss-view.xsl histograms.xml $(dependencies)
	$(xsltproc) $< | $(postprocess) >$@

stats-by-failure.tex: all_nf_none.xml stats-breakout.xsl $(dependencies)
	$(xsltproc) stats-breakout.xsl $< | $(postprocess) >$@

stats-by-increase.tex: all_lb_none.xml stats-breakout.xsl $(dependencies)
	$(xsltproc) stats-breakout.xsl $< | $(postprocess) >$@

stats-by-harmonic.tex: all_hl_none.xml stats-breakout.xsl $(dependencies)
	$(xsltproc) stats-breakout.xsl $< | $(postprocess) >$@

clean:
	rm -f rows.tex stats-by-*.tex

.DELETE_ON_ERROR:
