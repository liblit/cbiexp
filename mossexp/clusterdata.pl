#!/usr/bin/perl -w

use FileHandle;
use FindBin;

die "Do not run this program directly; use $FindBin::Bin instead\n" if @ARGV;

# allow complete group access
umask 002;

$DPATH = "/moa/sc1/cbi/sources";           # source code data is here
$MPATH = '../bin';			   # moss binaries are here
$watchdog = "$MPATH/watchdog.pl";
$ENV{'SAMPLER_DEBUGGER'} = "/usr/share/sampler/print-debug-info";  # collect stack traces
#
# This array contain a parameterized list of options.  Each option is
# a pair: a number and the option.  As perl does not support nested arrays
# well, the pairs are encoded as even and odd elements of a flat list.
# Each line has a list of pairs and is terminated by a "-1"; one line represents
# the distribution of possibilities for one command line option.  For each line and each trial, we
# generate a random number 1 <= x <= 100 and choose the option paired
# with the smallest number that is greater than x.
#
@parameters = (
	       5, "-c", 100, "", -1,                                                       # comments or no comments
	       5, "-s dbfile", 100, "", -1,                                                # save as a database file or not
	       2, "-r db1", 4, "-r db1 -r db2", 100, "", -1,                               # load one or two databases, or run normally
	       3, "-t 1000", 10, "-t 300", 90, "-t 30", 95, "-t 25", 100, "-t 5", -1,      # tile size
	       2, "-w 800", 5, "-w 200", 90, "-w 8", 95, "-w 5", 100, "-w 1", -1,          # winnowing window size
	       1, "-n 1", 4, "-n 2", 90, "-n 10", 100, "-n 1000000", -1,                   # max matches
	       1, "-p 10", 100, "", -1,                                                    # size of index in memory
	       -2 
              );

#
# This array is again a list of pairs (encoded again as even and odd elements).
# The first element is the cumulative probability, the second element is the number
# of files that will be submitted in the run.
#
@filedistribution = (2, 2, 4, 4, 6, 8, 10, 16, 50, 25, 80, 35, 99, 45, 100, 200);

#
# Again, cumulative probability and the programming language of the trial.
#  
@langdistribution = (5, "java", 10, "lisp", 100, "c");                                     

# The odds that we cannot write a database file.
$unwritable = 50;
 
#
# Lists of c, java, and lisp files for use in the trials.
# We choose elements from these lists randomly.
#
@c_files = `find $DPATH -name "*.c"`;
@java_files = `find $DPATH -name "*.java"`;
@lisp_files = `find $DPATH -name "*.lisp"`;


sub check_system ($) {
    die "system error: $!" if (system $_[0]) == -1;
    die "system command crashed" if $? & 127;
    return $? >> 8;
}


sub diff ($$) {
    my $status = check_system "cmp -s @_";
    die "cmp error on @_" if $status > 1;
    return $status;
}


sub run_moss ($$$) {
    my ($environment, $executable, $variant) = @_;

    # assemble the shell command
    my $command = "$watchdog 600 $variant $MPATH/$executable $full_option_list -a manifest >$variant/stdout 2>$variant/stderr";
    $command = "$environment $command" if $environment;

    # build data files directory
    mkdir $variant or die "mkdir $variant failed: $!";

    # record it for posterity
    my $commandHandle = new FileHandle "$variant/command", 'w';
    $commandHandle->print("$command\n");

    # run the command
    check_system $command;
}


sub run_moss_good () {
    run_moss '', 'moss', 'good';
}


sub run_moss_bad ($) {
    my $variant = shift;

    # run buggy moss and record various outputs
    run_moss "SAMPLER_FILE=$variant/reports", "moss$variant", $variant;

    # compare with reference
    my $outputDiff = diff "$variant/stdout", 'good/stdout';
    my $exitDiff = diff "$variant/exit", 'good/exit';
    my $failHandle = new FileHandle "$variant/fail", 'w';
    $failHandle->print($outputDiff || $exitDiff, "\n");
}


#
# Scan over the list of options and build up an option
# list according to the probability distribution.
#
$full_option_list = "";
$i = 0;
while ($parameters[$i] != -2) {
    $item = $parameters[$i++];
    $num = rand(100);
    $res = "";
    while ($item != -1) {
	$opt_string = $parameters[$i++];
	if ($num < $item) {
	    $res = $opt_string;
	    last;
	}
	$item = $parameters[$i++];
    }
    for(; $parameters[$i] ne -1; $i++) { }
    $i++;
    $full_option_list .= $res . " ";
}

#
# Now decide how many files will be submitted of which languages and choose
# them randomly.
#
$filenum = rand(100);
$i = 0;
$numfiles = 0;
while ($i < $#filedistribution) {
    $prob = $filedistribution[$i++];

    if ($filenum <= $prob) {
	$numfiles = $filedistribution[$i++];
	last;
    } else {
	$i++;
    }
}

$i = 0;
$lang = "";
$langnum = rand(100);
while ($i < $#langdistribution) {
    $prob = $langdistribution[$i++];

    if ($langnum <= $prob) {
	$lang = $langdistribution[$i++];
	last;
    } else {
	$i++;
    }
}

if ($lang eq "c") {
    @filelist = @c_files;
}
if ($lang eq "java") {
    @filelist = @java_files;
}
if ($lang eq "lisp") {
    @filelist = @lisp_files;
}

open(M,">manifest");
for($i = 1; $i < $numfiles+1; $i++) {
    $num = int(rand($#filelist));
    $file = $filelist[$num];
    chop($file);
    print M "$file $i $lang $file\n";
}
close(M);

#
# In the case of a trial that writes a database file, there is a chance the file is not writable.
#
if ($full_option_list =~ /dbfile/) {
    if (rand(100) < $unwritable) {
	check_system "touch dbfile; chmod a-wrx dbfile";
    }
}

#
# If we are supposed to read database db1, we need to build it.
# For this we use the standard moss executable so that there is no
# problem with data collection.
# 
if ($full_option_list =~ /db1/) {
    open(M,">db1.manifest");
    for($i = 1; $i < 20; $i++) {
	$num = int(rand($#filelist));
	$file = $filelist[$num];
	chop($file);
	print M "$file $i $lang $file\n";
    }
    close(M);
    check_system "$watchdog 1000 - $MPATH/moss $full_option_list -s db1 -a db1.manifest > output.db1 2> errors.db1";
}

#
# Likewise for db2; this is an ugly bit of cut and paste coding.
#
if ($full_option_list =~ /db2/) {
    open(M,">db2.manifest");
    for($i = 1; $i < 20; $i++) {
	$num = int(rand($#filelist));
	$file = $filelist[$num];
	chop($file);
	print M "$file $i $lang $file\n";
    }
    close(M);
    check_system "$watchdog 1000 - $MPATH/moss $full_option_list -s db2 -a db2.manifest > output.db2 2> errors.db2";
}

# various runs
run_moss_good;
run_moss_bad 'bad';
run_moss_bad 'bad2';

# overall sanity checks
my @abnormality;
my $goodExit = (new FileHandle 'good/exit')->getline;
push @abnormality, 'good run crashed' if $goodExit =~ /^signal\t/;
push @abnormality, 'good run timed out' if $goodExit eq 'timeout';
push @abnormality, 'bad and bad2 runs exited differently' if diff 'bad/exit', 'bad2/exit';
push @abnormality, 'bad run produced no reports' if -z 'bad/reports';
push @abnormality, 'bad2 run produced no reports' if -z 'bad2/reports';
if (@abnormality) {
    my $disregard = new FileHandle 'disregard', 'w';
    $disregard->print("$_\n") foreach @abnormality;
}
