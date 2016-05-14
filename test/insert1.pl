#!/usr/bin/perl

use vars;
use warnings;
use autodie;

push @INC, "./test/";
require testlib;
require randtable;

$row_count = int(shift(@ARGV) || 1000);
$group_size = int(shift(@ARGV) || ($row_count / 100));

running("insert1");
open_temp_dir();
find_zolden();
start_server();

$csvfilename_unsorted = "$tempdir/data1.csv";
$csvfilename_sorted = "$tempdir/data2.csv";
$csvfilename_got = "$tempdir/data3.csv";
$shfilename_insert = "$tempdir/insert.sh";
open $csvfile, ">", $csvfilename_unsorted;
open $shfile, ">", $shfilename_insert;

$ingroup_count = 0;
$query_begin = "echo";
$query_end = " | $zol &\n";

$cb = sub {
	print $csvfile join(",", @_), "\n";
	$teacher = shift @_;
	$subject = shift @_;
	$room = shift @_;
	$group = shift @_;
	$day = shift @_;
	$lesson = shift @_;

	$query = "insert teacher = \"$teacher\", subject = \"$subject\", room = $room, group = $group, day = $day, lesson = $lesson;";
	print $shfile " '$query'\\\n";
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
maketable( \$cb, $row_count, 100, 1000);
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

print STDERR "Selecting\n";
system "echo 'print;' | $zol | ./test/print2csv.pl | sort -u > $csvfilename_got";

print STDERR "Comparing\n";
$result = system "diff -q $csvfilename_sorted $csvfilename_got";
if($result == 0) {
	print STDERR "Test passed\n";
	pass("$row_count rows in groups of $group_size rows each\n");
	stop_server();
	close_temp_dir();
} else {
	print STDERR "Test failed\n";
	fail("Database engine error (or external influence)\n");
}
