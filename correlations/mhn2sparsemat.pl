#!/usr/bin/perl -w

use strict;

my $usage = "./mhn2sparsemat.pl <datadir> <outdir>";
my $datadir = shift;
my $outdir = shift;

die $usage if !defined $datadir || !defined $outdir;

`mkdir -p $outdir` if !-e $outdir;

print "Reading info...\n";
my ($farr, $fhash) = read_info("$datadir/f.runs");
my ($sarr, $shash) = read_info("$datadir/s.runs");
my ($parr, $phash) = read_info("$datadir/preds.txt");
my $npreds = scalar(keys %{$phash});

print "Reading counts...\n";
my ($ftr, $str) = read_counts("$datadir/tru.txt");
my ($fobs, $sobs) = read_counts("$datadir/obs.txt");

print "Outputting sparse matrix ftr\n";
output_sparse("$outdir/ftr", $ftr, $farr, $fhash);
print "Outputting sparse matrix fobs\n";
output_sparse("$outdir/fobs", $fobs, $farr, $fhash);
print "Outputting sparse matrix str\n";
output_sparse("$outdir/str", $str, $sarr, $shash);
print "Outputting sparse matrix sobs\n";
output_sparse("$outdir/sobs", $sobs, $sarr, $shash);

sub output_sparse {
  my ($fn, $rlist, $arr, $hash) = (@_);
  open (DATA, "> $fn.") || die "Open: $!";
  open (IR, "> $fn.ir") || die "Open: $!";
  open (JC, "> $fn.jc") || die "Open: $!";
  open (META, "> $fn.meta") || die "Open: $!";
  my $row = 0;
  my $ctr = 0;
  print IR "$ctr\n";
  for my $p (0..scalar(@{$rlist})-1) {
    my $rs = $rlist->[$p];
    $row++;
    next if !defined $rs;
    my (@runs) = split / /, $rs;
    for my $r (@runs) {
      my $ind = $hash->{$r} || die "No run defined for $r";
      print DATA "1\n";
      print JC $ind-1, "\n";
    }
    $ctr += scalar(@runs);
    print IR "$ctr\n";
  }
  print META "nruns = ", scalar(keys %{$hash}), "\nnpreds = $npreds\nnzmax = $ctr\n";
  close (JC);
  close (IR);
  close (DATA);
}

sub read_counts {
  my ($fn) = (@_);
  my (@farr, @sarr);
  open (IN, "$fn") || die "Open: $!";
  while (<IN>) {
    chomp;
    my $key = $_;
    my $k = $phash->{$key};
    die "No index for $key" if !defined $k;
    die "Duplicate pred $key" if defined $farr[$k-1];
    die "Duplicate pred $key" if defined $sarr[$k-1];
    $_ = <IN>;
    chomp; s/^F: //; s/\s+$//;
    $farr[$k-1] = $_; 
    $_ = <IN>;
    chomp; s/^S: //; s/\s+$//;
    $sarr[$k-1] = $_;
  }
  close (IN);

  return (\@farr, \@sarr);
}

sub read_info {
  my ($fn) = (@_);
  my @info;
  my %info;
  my $k = 1;
  open (INFO, "$fn") || die "Open: $!";
  while (<INFO>) {
    chomp;
    push @info, $_;
    $info{$_} = $k++;
  }
  close (INFO);

  return (\@info, \%info);
}
