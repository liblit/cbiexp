package TimedCommand;

use strict;


########################################################################


sub new ($@) {
    my $proto = shift;
    my $class = ref($proto) || $proto;

    warn "launching timed command @_\n";

    my $pid = fork;
    die "cannot fork: $!" unless defined $pid;

    if ($pid == 0) {
	# child process
	exec { $_[0] } @_;
	die "cannot exec $_[0]: $!";
    }

    my $self = \$pid;
    bless $self, $class;
    return $self;
}


sub wait ($$) {
    my ($self, $timeout) = @_;
    my $status;

    eval {
	local $SIG{ALRM} = sub { die "alarm\n" };
	alarm $timeout;
	waitpid $$self, 0;
	$status = $?;
	alarm 0;
    };

    if ($@) {
	die unless $@ eq "alarm\n";
	# child still running
	return 'timeout';
    } else {
	# child exited on its own
	return 'exit', $status;
    }
}


sub kill ($$) {
    my ($self, $signal) = @_;
    kill $signal, $$self;
}


########################################################################


1;
