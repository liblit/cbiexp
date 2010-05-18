OTHERFILES = $(wildcard *.*)

include $(WEBRULES)

faq:
	/project/star/vnc/scripts/makefaq.py < faq.xml > faq.html

install::
	(cd /project/star/vnc/external/vnc-3.3.3/; \
	 ./make-zips doc && ./install-zips doc)
