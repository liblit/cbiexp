ranking := all_hl_corrected-exact-complete.xml

all: rows.tex stats-by-failure.tex stats-by-increase.tex stats-by-harmonic-dilute.tex

rows.tex: $(ranking) corrected-moss-view.xsl bug-o-meter.xsl ../../src/operands.xsl predictor-info.xml histograms.xml postprocess
	xsltproc --path ../../src $< | ./postprocess >$@

stats-by-failure.tex: all_lbnf_none.xml stats-breakout.xsl bug-o-meter.xsl ../../src/operands.xsl predictor-info.xml postprocess
	xsltproc --path ../../src stats-breakout.xsl $< | ./postprocess >$@

stats-by-increase.tex: all_lb_none.xml stats-breakout.xsl bug-o-meter.xsl ../../src/operands.xsl predictor-info.xml postprocess
	xsltproc --path ../../src stats-breakout.xsl $< | ./postprocess >$@

stats-by-harmonic-dilute.tex: $(ranking) stats-breakout.xsl bug-o-meter.xsl ../../src/operands.xsl predictor-info.xml postprocess
	xsltproc --param predictor-limit 30 --path ../../src stats-breakout.xsl $< | ./postprocess >$@

clean:
	rm -f rows.tex

.DELETE_ON_ERROR:
