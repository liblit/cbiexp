package TaggedLoader;

use strict;

use Carp;


########################################################################


sub new ($$) {
    @_ == 2 or confess 'wrong argument count';
    my ($proto, $element) = @_;
    my $class = ref($proto) || $proto;
    my $self = { element => $element };
    bless $self, $class;
}


sub find ($$$) {
    @_ == 3 or confess 'wrong argument count';
    my ($self, $unit, $scheme) = @_;

    return $self->{sections}{$unit}{$scheme};
}


sub foreach ($\&) {
    @_ == 2 or confess 'wrong argument count';
    my ($self, $handler) = @_;

    foreach my $unit (sort keys %{$self->{sections}}) {
	foreach my $scheme (sort keys %{$self->{sections}{$unit}}) {
	    $handler->($unit, $scheme, $self->{sections}{$unit}{$scheme});
	}
    }
}


sub decode_start_tag ($$) {
    @_ == 2 or confess 'wrong argument count';
    my ($self, $received) = @_;

    $received =~ /\A<$self->{element} unit="([0-9a-f]{32})" scheme="([-a-z]+)">\Z/
	or die "malformed $self->{element} start tag: $received\n";

    return ($1, $2);
}


sub read ($$$) {
    @_ == 3 or confess 'wrong argument count';
    my ($self, $source, $handle) = @_;

    while (my $tag = <$handle>) {
	my ($unit, $scheme) = $self->decode_start_tag($tag);

	my @lines;
	while (my $line = <$handle>) {
	    last if $line =~ /\A<\/$self->{element}>\Z/;
	    chomp $line;
	    push @lines, $line;
	}

	my $prior = $self->{sections}{$unit}{$scheme};
	my $merged;

	if (defined $prior) {
	    $merged = $self->merge($prior->{lines}, \@lines);
	    die "cannot merge multiple instances of unit $unit, scheme $scheme\n" unless $merged;
	} else {
	    $merged = \@lines;
	}

	$merged = { source => $source, lines => $merged };
	$self->{sections}{$unit}{$scheme} = $merged;
    }
}


########################################################################


1;
