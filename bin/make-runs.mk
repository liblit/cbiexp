stamp-labels:
	echo $(num_runs) >$@

%/label: %/stamp $(label_once)
	echo run $*: label
	cd $* && $(label_once) $(notdir $*) >$(@F).
	mv $@. $@

%/stamp:
	test ! -e ../orderly-shutdown
	echo run $*: start
	if [ -d $* ]; then rm -r $*; fi
	mkdir -p $*
	cd $* && $(run_once) $(notdir $*)
	echo run $*: done
	touch $@

.PRECIOUS: %/stamp
