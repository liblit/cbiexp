#!/usr/bin/perl -w


use FileHandle;
use IO::Select;
use POSIX qw(SIGHUP SIGINT SIGKILL);
use strict;


########################################################################


my $timeout = shift;
my $variant = shift;


########################################################################


$| = 1;

my $start = time;

my $handle = new FileHandle;
my $pid = $handle->open("@ARGV |");
my $select = new IO::Select $handle;
my $ready;
while ($ready = $select->can_read($timeout)
       and local $_ = <$handle>) {
    print $_;
}

my $elapsed = time - $start;
my $timeHandle = new FileHandle "$variant/time", 'w';
$timeHandle->print("$elapsed\n");

my $exitHandle = new FileHandle "$variant/exit", 'w';
if ($ready) {
    $handle->close;
    my $signal = $? & 127;
    my $status = $? >> 8;
    if ($signal) {
	$exitHandle->print("signal\t$signal\n");
	exit $signal;
    } else {
	$exitHandle->print("normal\t$status\n");
	exit $status;
    }
} else {
    kill SIGKILL, $pid;
    $exitHandle->print("timeout\n");
    exit 123;
}
