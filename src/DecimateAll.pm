package DecimateAll;

use FileHandle;
use IO::Pipe;
use RunsRunner;

our @ISA = qw(RunsRunner);


########################################################################


sub new ($%) {
    my $proto  = shift;
    my $class = ref($proto) || $proto;
    my $self = bless $proto->SUPER::new(@_, description => 'decimating reports'), $class;
    return $self;
}


sub run_task ($$$) {
    my ($self, $runId) = @_;
    $0 = "decimate $runId";

    my $rundir = $self->rundir($runId);
    my $input = "$rundir/reports.sparse";
    return unless -r $input;

    my $pipe = new IO::Pipe;
    my $pid = fork;
    die "cannot fork: $!\n" unless defined $pid;

    if ($pid) {
	$pipe->reader;

	my $output = "$input.$self->{sparsity}";
	$output .= '-ex' if @{$self->{exempts}};
	my $filtered = new FileHandle $output, 'w' or die "cannot write $output: $!\n";

	while (<$pipe>) {
	    $filtered->print($_) unless /^\d+\t\d+(\t0)*\n$/;
	}

	die "pipe close failed: $!\n" unless $pipe->close;
	die "wait failed: $!\n" unless wait == $pid;
	die "subprocess failed\n" if $?;

    } else {
	$pipe->writer;
	open STDIN, '<', "$input" or die "cannot read $input: $!\n";
	open STDOUT, '>&', $pipe or die "cannot redirect stdout into pipe: $!\n";
	my @command = $self->{decimator};
	push @command, '--sparsity', $self->{sparsity};
	push @command, '--exempt-sites', $_ foreach @{$self->{exempts}};
	exec @command;
	die "cannot spawn $decimator: $!\n";
    }

    return 0;
}


########################################################################


1;
