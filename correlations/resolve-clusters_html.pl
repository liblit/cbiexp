#!/usr/bin/perl -w

use strict;

my $usage = "./resolve-clusters_html.pl <datadir>";
my $datadir = shift;
die $usage unless defined $datadir;

my $pfiledir = "$datadir/html";
if (!-e $pfiledir) {
  `mkdir -p $pfiledir`;
  `ln -s /moa/sc3/mhn/$datadir/\*.html $pfiledir/.`;
  `ln -s /moa/sc3/mhn/$datadir/r $pfiledir/.`;
  `ln -s /moa/sc3/mhn/$datadir/s $pfiledir/.`;
}

my $header = "<html>\n<body>\n";
my $footer = "</body>\n</html>\n";

## read in the predicate statistics first
my @plist;
push @plist, &read_htmlinfo("$pfiledir/B.html") if -e "$pfiledir/B.html";
push @plist, &read_htmlinfo("$pfiledir/R.html") if -e "$pfiledir/R.html";
push @plist, &read_htmlinfo("$pfiledir/S.html") if -e "$pfiledir/S.html";
die "Oops" if $#plist < 3;

sub read_htmlinfo {
  my ($fn) = (@_);
  my @list;
  open (IN, "$fn") || die "Open: $!";
  #$_ = <IN>; $_ = <IN>;  #skip header
  while (<IN>) {
    next if /^</;  # skip any html tag lines
    last if /<\/body>/;
    chomp;
    push @list, $_;
  }
  close (IN);

  return (@list);
}

open (OUT, "> $pfiledir/speccluster.html") || die "Open: $!";
print OUT "$header";
my $k = 0;
while (<>) {
  chomp;
  $k++;
  print OUT "Cluster $k:<br>\n";
  next if /^$/;
  my ($p1, $str) = split /:/;
  die "Can't find predicate $p1" if !defined $plist[$p1-1];
  print OUT $plist[$p1-1], "\n";;
  $str =~ s/^\s+//;
  $str =~ s/\s+$//;
  my (@p2list) = split / /, $str;
  my $i = 0;
  while ($i < $#p2list) {
    my $p2 = $p2list[$i];
    my $cscore = $p2list[$i+1];
    $i += 2;
    next if $p2 == $p1;
    die "Can't find predicate $p2" if !defined $plist[$p2-1];
    print OUT "****$cscore COR ", $plist[$p2-1], "\n";
  }
  print OUT "<br>\n";
}
print OUT "$footer\n";
close (OUT);
