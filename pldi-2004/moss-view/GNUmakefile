ranking := all_hl_corrected-exact-complete.xml

rows.tex: $(ranking) corrected-moss-view.xsl ../../src/operands.xsl predictor-info.xml histograms.xml postprocess
	xsltproc --path ../../src $< | ./postprocess >$@

clean:
	rm -f rows.tex

.DELETE_ON_ERROR:
