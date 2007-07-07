#!/usr/bin/perl -w

use strict;
use List::Util 'shuffle';

my $usage = "./genFig1.pl <dataversion>";
my $dataversion = shift;
die $usage if !defined $dataversion;

my $outputdir = "figuredata/ds$dataversion";
`mkdir $outputdir` if !-e $outputdir;
my $prog = "./compute_complete";

my $srunsfn = "/moa/sc3/mhn/moss/data10/tmp/s.runs.new";
my $frunsfn = "/moa/sc3/mhn/moss/data10/tmp/f.runs.new";
my $sruns = &read_list ($srunsfn);
my $fruns = &read_list ($frunsfn);

my @slist = keys %{$sruns};
my @flist = keys %{$fruns};
my @list = @flist;
push @list, @slist;
my $numfiles = scalar(@list);
print "Running on database ds$dataversion, outputdir $outputdir, on $numfiles files\n";

for my $t (1..10) {
  my @shuffled = shuffle(@list); 
  my $sttime; my $endtime;
  $sttime = time;

  my $outputpref = "$outputdir/$t";
  my $outorder = "$outputpref-order.txt";
  my $outcount = "$outputpref-count.txt";
  open (OUT, "> $outorder") || die "Open: $!";
  foreach (@shuffled) {
    print OUT;
    print OUT "\n";
  }
  close (OUT);
  
  print "run $t\n";

  ## run the computation routine
  `$prog -s $srunsfn -f $frunsfn -p $outorder -d $dataversion > $outcount`;
  $endtime = time;
  print "  ", $endtime-$sttime, " seconds\n";

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
