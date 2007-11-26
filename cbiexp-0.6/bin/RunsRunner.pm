package RunsRunner;

use strict;

use TaskRunner;

our @ISA = qw(TaskRunner);


########################################################################


my $RunsPerSubdirectory = 10000;


sub rundir ($$) {
    my ($self, $runId) = @_;
    my $subdir = int($runId / $RunsPerSubdirectory);
    return "$self->{datadir}/$subdir/$runId";
    my $rundir = $self->rundir($runId);
}


########################################################################


1;
