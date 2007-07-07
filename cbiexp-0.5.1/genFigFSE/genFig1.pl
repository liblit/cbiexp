#!/usr/bin/perl -w

use strict;
use List::Util 'shuffle';

my $usage = "./genFig1.pl <dataversion>";
my $dataversion = shift;
die $usage if !defined $dataversion;

my $outputdir = "figuredata/ds$dataversion";
`mkdir $outputdir` if !-e $outputdir;
my $prog = "./compute";

my $sruns = &read_list ("/moa/sc3/mhn/moss/data10/tmp/s.runs");
my $fruns = &read_list ("/moa/sc3/mhn/moss/data10/tmp/f.runs");

my @slist = keys %{$sruns};
my @flist = keys %{$fruns};
my @list = @flist;
push @list, @slist;
my $numfiles = scalar(@list);
print "Running on database ds$dataversion, outputdir $outputdir, on $numfiles files\n";

for my $t (1..10) {
my @shuffled = shuffle(@list); 
open (OUT, "> order.txt") || die "Open: $!";
foreach (@shuffled) {
  print OUT;
  print OUT "\n";
}
close (OUT);

my $chunksize = 3000;
my $start = 0;
my $end = 0;
my $sttime; my $endtime;
my $ctr = 1;
while ($end < $numfiles) {
  $sttime = time;
  $end += $chunksize;
  $end = $numfiles if $end > $numfiles;
  my $outputpref = "$outputdir/$t-$ctr";
  my $flistout = "$outputpref-flist.txt";
  my $slistout = "$outputpref-slist.txt";
  my $out = "$outputpref-count.txt";

  print "run $t, chunk $ctr, start $start, end $end\n";
  open (FOUT, ">$flistout") || die "Open: $!";
  open (SOUT, ">$slistout") || die "Open: $!";
  foreach my $r (@shuffled[$start..$end-1]) {
    print FOUT "$r\n" if defined $fruns->{$r};
    print SOUT "$r\n" if defined $sruns->{$r};
  }
  close (FOUT);
  close (SOUT);

  ## run the computation routine
  `$prog -s $slistout -f $flistout -d $dataversion > $out`;
  $endtime = time;
  print "  ", $endtime-$sttime, " seconds\n";

  $ctr++;
}
}

sub read_list {
  my ($file) = (@_);
  open (FH, "$file") || die "Open $file: $!";
  my %runlist;
  while (<FH>) {
    chomp;
    $runlist{$_} = 1;
  }
  close (FH);
  return \%runlist;
}
