package TaskRunner;

use strict;


########################################################################


sub cpu_count ($) {
    my $count = 0;
    my $cpuinfo = new FileHandle '/proc/cpuinfo';
    while (<$cpuinfo>) {
	++$count if /^processor\t/;
    }
    return $count || 1;
}


########################################################################


sub new ($%) {
    my $proto = shift;
    my $class = ref($proto) || $proto;
    my %self = @_;
    my $self = \%self;

    die "job count not set" unless $self->{jobs};
    $self->{fancy} = -t && $ENV{TERM};

    bless ($self, $class);
    return $self;
}


sub necessary_task ($$) {
    return 1;
}


sub run_task ($$) {
    my $self = shift;
    die "$self: subclass must implement \"run_task\" method\n";
}


sub error ($) {
    my $self = shift;
    $self->{error} = 1;
}


sub progress ($) {
  my $self = shift;
  my $percent = int(100 * $self->{step} / $self->{total});
  my $before = $self->{fancy} ? "\r" : '';
  my $after = $self->{fancy} && $self->{step} != $self->{total} ? '' : "\n";
  local $| = 1;
  print "$before$self->{description}: $self->{step}/$self->{total} $percent%$after";
}


sub reap ($) {
    my $self = shift;
    wait;
    $self->error if $?;
    --$self->{children};
    $self->{updated} = 1;
}


sub run_tasks ($@) {
    my $self = shift;

    $self->{error} = 0;
    $self->{children} = 0;
    $self->{step} = 0;
    $self->{total} = @_;

    foreach my $task (@_) {
	if (-e ('orderly-shutdown')) {
	    $self->error;
	    last;
	}

	++$self->{step};

	# is any work needed for this task?
	next unless $self->necessary_task($task);

	# wait for an available slot
	$self->reap while $self->{children} >= $self->{jobs};
	last if $self->{error};
	$self->progress;

	# use the available slot
	my $pid = fork;
	die "cannot fork: $!\n" unless defined $pid;

	if ($pid == 0) {
	    # child
	    my $result = $self->run_task($task);
	    exit $result if defined $result;
	    exit 0;
	} else {
	    # parent
	    ++$self->{children};
	}
    }

    # finish up any remaining jobs
    $self->reap while $self->{children};
}


########################################################################


1;
