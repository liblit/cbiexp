########################################################################
#
#  assorted code fragments which are useful in "label-once" scripts
#

package Check;


use strict;
use Carp;
use FileHandle;


########################################################################


sub stamp() {
    croak "wrong argument count" unless @_ == 0;
    die "run is still in progress\n" unless -e 'stamp';
}


sub reports () {
    croak "wrong argument count" unless @_ == 0;

    # does reports file exist?
    my $handle = new FileHandle 'reports', 'r';
    return 'ignore' unless $handle;

    # is reports file empty or malformed?
    my $last;
    $last = $_ while <$handle>;
    return 'ignore' unless defined $last;
    return 'ignore' unless $last eq "</report>\n";

    return undef;
}


sub outcome ($$) {
    croak "wrong argument count" unless @_ == 2;
    my ($filename, $errorResult) = @_;

    # was an outcome properly recorded?
    my $outcome = new FileHandle $filename, 'r';
    return 'ignore' unless $outcome;
    my $status = $outcome->getline;
    return 'ignore' unless defined $status;

    # normal termination with no error code
    return undef if $status == 0;

    # abnormal termination due to fatal signal
    return 'failure' if $status < 256;

    # normal termination with error code
    return $errorResult;
}


########################################################################


1;
