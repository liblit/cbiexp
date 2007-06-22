#!/usr/bin/perl -w

use strict;

my $usage = "./genFig2.pl <dataversion>";
my $dataversion = shift;
die $usage if !defined $dataversion;

my $datadir = "./figuredata/ds$dataversion";
my $outfile = "results$dataversion.txt";
die "No datadir $datadir" if !-e $datadir;

open (my $out, ">$outfile") || die "Open $outfile: $!";
for my $t (1..10) {
  if (-e "$datadir/$t-count.txt") {
    &record_complete($out, $t);
  }
  else {
    &record_sep($out, $t);
  }
}
close ($out);

sub record_complete {
  my ($outfd, $t) = (@_);
  open (my $in, "$datadir/$t-count.txt") || die "Open: $!";
  $_ = <$in>;
  for my $ctr (1..11) {
    my ($nb, $nr, $ns) = &read_counts($in);
    print $outfd "$t $ctr $nb $nr $ns\n";
  }
  close ($in);
}

sub record_sep {
  my ($outfd, $t) = (@_);
  for my $ctr (1..11) {
    open(my $in, "$datadir/$t-$ctr-count.txt") || die "Open: $!";
    my ($nb, $nr, $ns) = &read_counts($in);
    close ($in);
    print $outfd "$t $ctr $nb $nr $ns\n";
  }
}

sub read_counts {
  my ($in) = (@_);
  $_ = <$in>; ## first line is the count of the number of runs of each kind
  $_ = <$in>; chomp;
  my (undef, $nb) = split /: /;
  $_ = <$in>; chomp;
  my (undef, $nr) = split /: /;
  $_ = <$in>; chomp;
  my (undef, $ns) = split /: /;
  return ($nb, $nr, $ns);
}
