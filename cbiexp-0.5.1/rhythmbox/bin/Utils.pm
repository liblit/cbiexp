########################################################################
#
#  assorted small utility routines that don't fit better elsewhere

package Utils;


use strict;

use FileHandle;


########################################################################
#
#  set output file prefix for use in subsequent commands
#

our $role;

sub role ($) {
    $role = shift;
    mkdir $role or die "cannot mkdir $role: $!";
}


########################################################################
#
#  log standard output and error
#

sub redirect () {
    open STDOUT, ">$role/stdout" or die "cannot redirect stdout: $!";
    open STDERR, ">$role/stderr" or die "cannot redirect stderr: $!";
}


########################################################################
#
#  record some small fact in an external file
#

sub note ($@) {
    my ($filename, @message) = @_;
    my $path = "$role/$filename";
    my $handle = new FileHandle $path, 'w' or warn "cannot write to $path: $!";
    $handle->print("@message\n");
}


########################################################################


1;
