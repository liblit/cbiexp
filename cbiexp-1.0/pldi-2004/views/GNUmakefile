ranking := all_hl_corrected-exact-complete.xml
apps := bc ccrypt exif rhythmbox
rows := $(apps:=/rows.tex)

all: $(rows)

$(rows): %/rows.tex: %/$(ranking) corrected-view.xsl bug-o-meter.xsl ../../src/operands.xsl %/predictor-info.xml postprocess
	xsltproc --path '$* . ../../src' $< | ./postprocess >$@

clean:
	rm -f $(rows)

.DELETE_ON_ERROR:
