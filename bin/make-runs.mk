stamp-labels:
	echo $(num_runs) >$@

%/label: %/stamp $(label_once)
	echo run $(*F): label
	cd $* && $(label_once) $(notdir $*) >$(@F).
	mv $@. $@

%/stamp:
	test ! -e ../orderly-shutdown
	echo run $(*F): start
	if [ -d $* ]; then rm -r $*; fi
	mkdir -p $*
	cd $* && $(run_once) $(*F)
	echo run $(*F): done
	touch $@

.PRECIOUS: %/stamp
