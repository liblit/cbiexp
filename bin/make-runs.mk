stamp-labels:
	touch $@

%/label: %/stamp $(label_once)
	echo run $*: label
	cd $* && $(label_once) $* >$(@F).
	mv $@. $@

%/stamp:
	test ! -e ../orderly-shutdown
	echo run $*: start
	if [ -d $* ]; then rm -r $*; fi
	mkdir $*
	cd $* && $(run_once) $*
	echo run $*: done
	touch $@

.PRECIOUS: %/stamp
