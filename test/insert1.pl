#!/usr/bin/perl

use strict;
use strict qw(vars refs);
use vars;
use warnings;
use autodie;

push @INC, "./test/";
require testlib;
require randtable;

our $row_count = int(shift(@ARGV) || 1000);
our $group_size = int(shift(@ARGV) || ($row_count / 100));

running("insert1");
find_zolden();
my $tempdir = open_temp_dir();
my $datadir = "$tempdir/data";
mkdir $datadir;
my $zol = start_server($datadir);

my $csvfilename_unsorted = "$tempdir/data1.csv";
my $csvfilename_sorted = "$tempdir/data2.csv";
my $csvfilename_got = "$tempdir/data3.csv";
my $shfilename_insert = "$tempdir/insert.sh";
open our $csvfile, ">", $csvfilename_unsorted;
open our $shfile, ">", $shfilename_insert;

our $ingroup_count = 0;
our $query_begin = "$zol <<'QUERY_END' &\n";
our $query_end = "QUERY_END\n";

my $cb = sub {
	print $csvfile join(",", @_), "\n";
	my $teacher = shift @_;
	my $subject = shift @_;
	my $room = shift @_;
	my $group = shift @_;
	my $day = shift @_;
	my $lesson = shift @_;

	my $query = "insert teacher = \"$teacher\", subject = \"$subject\", room = $room, group = $group, day = $day, lesson = $lesson;";
	print $shfile "\t$query\n";
	if(++$ingroup_count >= $group_size) {
		print $shfile $query_end;
		print $shfile $query_begin;
		$ingroup_count = 0;
	}
};

print STDERR "Generating the table\n";
print $shfile "#!/bin/bash\n";
print $shfile "\n";
print $shfile "\n";
print $shfile $query_begin;
maketable( \$cb, $row_count);
print $shfile $query_end;
print $shfile "\n";
print $shfile "wait\n";

close $csvfile;
close $shfile;
chmod 0700, $shfilename_insert;

print STDERR "Sorting\n";
system "sort -u $csvfilename_unsorted > $csvfilename_sorted";

print STDERR "Inserting\n";
system "$shfilename_insert >/dev/null";

$zol = restart_point();

print STDERR "Selecting\n";
system "$zol <<<'print;' | ./test/print2csv.pl | sort > $csvfilename_got";

stop_server();

print STDERR "Comparing\n";
my $result = system "diff -q $csvfilename_sorted $csvfilename_got";
if($result == 0) {
	print STDERR "Test passed\n";
	pass("$row_count rows in groups of $group_size rows each\n");
	close_temp_dir();
} else {
	print STDERR "Test failed\n";
	fail("Database engine error (or external influence)\n");
}
