package MakeRuns;

use Cwd;
use File::Path;
use FileHandle;
use RunsRunner;

our @ISA = qw(RunsRunner);


########################################################################


sub new ($@) {
    my $proto  = shift;
    my $class = ref($proto) || $proto;
    my $self = bless $proto->SUPER::new(@_, description => 'making runs'), $class;

    my $cwd = getcwd;
    $self->{run_once} = "$cwd/bin/run-once";
    $self->{label_once} = "$cwd/bin/label-once";
    $self->check_script($self->{run_once});
    $self->check_script($self->{label_once});

    return $self;
}


sub check_script ($$) {
    my ($self, $script) = @_;
    die "$script script is missing\n" unless -e $script;
    die "$script script is not executable\n" unless -x $script;
}


sub necessary_task ($$$) {
    my ($self, $runId) = @_;
    my $rundir = $self->rundir($runId);
    return ! -e "$rundir/label";
}


sub run_task ($$$) {
    my ($self, $runId) = @_;
    $0 = "run $runId";

    # prepare a fresh run directory
    my $rundir = $self->rundir($runId);
    rmtree $rundir;
    mkpath $rundir;
    chdir $rundir;

    # run the application
    system $self->{run_once}, $runId;
    return 1 if $?;

    # record that the run completed
    new FileHandle 'stamp', 'w' or die "cannot update stamp: $!\n";

    # label the outcome
    open STDOUT, '>', 'label' or die "cannot write label: $!\n";
    system $self->{label_once}, $runId;
    if ($?) {
	unlink 'label';
	return 1;
    } else {
	return 0;
    }
}


########################################################################


1;
