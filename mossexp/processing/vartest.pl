#!/usr/bin/perl

$current_fun = "";
$current_var = "";
$varid = $ARGV[0];    # filter based on variable 1 or variable 2? 
$global = $ARGV[1];   # treat all variable names as global (1) or local (0)?

while(<STDIN>) {
    $rec = $_;
    ($succ, $fail, $fun, $line, $cfg, $var1, $rel, $var2) = split(' ',$rec,8);
    if ($varid eq 1) {
	$var = $var1;
    } else {
	$var = $var2;
    }
    if (($var eq $current_var) && (($global == 1) || ($fun eq $current_fun))) {
	$drop = 0;
	foreach $r (@scoreprev) {
	    ($psucc,$pfail,$rest) = split(' ',$r,3);
	    if (($psucc <= $succ) && ($pfail >= $fail)) {
		$drop = 1;
	    }
	}
	if ($drop == 0) {
	    print $rec;
	    push(@scoreprev,($rec));
	}
    } else {
	@scoreprev = ($rec);
	print $rec;
    }
    $current_var = $var;
    $current_fun = $fun;
}
	
