#!/usr/bin/perl

$threshold = $ARGV[0];
while(<STDIN>) {
    $rec = $_;
    ($succ, $fail, $fun, $line, $cfg, $var1, $rel, $var2) = split(' ',$rec,8);
    if ($fail > $threshold) {
	print $rec;
    }
}
