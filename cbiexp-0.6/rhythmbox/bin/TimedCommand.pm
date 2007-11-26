package TimedCommand;

use strict;


########################################################################


sub new ($@) {
    my $proto = shift;
    my $class = ref($proto) || $proto;

    my $self = {};
    bless $self, $class;

    $self->{command} = \@_;
    $self->trace("launch @_");

    $self->{pid} = fork;
    die "cannot fork: $!" unless defined $self->{pid};

    if ($self->{pid} == 0) {
	# child process
	exec { $_[0] } @_;
	die "cannot exec $_[0]: $!";
    }

    $self->trace("pid $self->{pid}");
    return $self;
}


sub wait ($$) {
    my ($self, $timeout) = @_;
    my $status;
    $self->trace("wait $timeout");

    eval {
	local $SIG{ALRM} = sub { die "alarm\n" };
	alarm $timeout;
	waitpid $self->{pid}, 0;
	$status = $?;
	alarm 0;
    };

    if ($@) {
	die unless $@ eq "alarm\n";
	# child still running
	$self->trace('timed out');
	return 'timeout';
    } else {
	# child exited on its own
	$self->trace("exited with status $status");
	return 'exit', $status;
    }
}


sub kill ($$) {
    my ($self, $signal) = @_;
    $self->trace("kill $signal");
    kill $signal, $self->{pid};
}


sub trace($@) {
    my $self = shift;
    warn "$self: ", @_, "\n";
}


########################################################################


1;
