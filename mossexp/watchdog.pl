#!/usr/bin/perl


use FileHandle;
use IO::Select;
use POSIX qw(SIGHUP SIGINT SIGKILL);
use strict;


########################################################################


sub status ($) {
    print STDERR scalar localtime, '  --  watchdog: ', shift, "\n";
}


########################################################################


my $timeout = shift(@ARGV);

########################################################################


$| = 1;

my $handle = new FileHandle;
my $pid = $handle->open("@ARGV |");
my $select = new IO::Select $handle;
my $ready;
while ($ready = $select->can_read($timeout)
       and local $_ = <$handle>) {
    print $_;
}

if (!$ready) {
    status 'killing';
    kill SIGKILL, $pid;
}

$handle->close;
exit($? >> 8 || $?);
