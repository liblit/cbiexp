filtered_on_var2.tmp: var2.tmp
	vartest.pl 2 1 < var2.tmp > filtered_on_var2.tmp

var2.tmp: filtered_on_var1.tmp
#	sort -k8,8 -k3,3 -k2,2nr -k1,1n < filtered_on_var1.tmp > var2.tmp
	sort -k8,8 -k2,2nr -k1,1n < filtered_on_var1.tmp > var2.tmp

filtered_on_var1.tmp: var1.tmp
	vartest.pl 1 1 < var1.tmp > filtered_on_var1.tmp

var1.tmp: low_fails_removed.tmp
#	sort -k6,6 -k3,3 -k2,2nr -k1,1n < low_fails_removed.tmp > var1.tmp
	sort -k6,6 -k2,2nr -k1,1n < low_fails_removed.tmp > var1.tmp

low_fails_removed.tmp: latest.txt
#	failfilter.pl 50 < latest.txt > low_fails_removed.tmp
	cat latest.txt > low_fails_removed.tmp

clean:
	rm *.tmp *~

