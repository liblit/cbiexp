root := ../..
experiment := ..
datadir := $(experiment)/data

sites := $(experiment)/share/rhythmbox.sites

all: stamp-dynamic

stamp-dynamic: dynamic $(datadir)/stamp-labels stamp-static
	./$<
	touch $@

stamp-static: static $(sites)
	./$< $(sites:%=-st %)
	touch $@

clean: clean-dynamic clean-static
.PHONY: clean

clean-dynamic:
	rm -f stamp-dynamic
	rm -f *.xml *.dtd *.xsl *.css
	rm -f compute-results convert-reports gen-views
	rm -f f.runs s.runs
.PHONY: clean-dynamic

clean-static:
	rm -f stamp-static
	rm -f sites.cc sites.o units.cc units.o
.PHONY: clean-static
