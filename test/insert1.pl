#!/bin/perl

use autodie;
use File::Temp qw/ tempfile tempdir /;

push @INC, "./test/";

require randtable;

$gray = "\e[m";
$red = "\e[31m";
$green = "\e[32m";
$pass = "[${green}PASS${gray}]";
$fail = "[${red}FAIL${gray}]";

$testname = "insert1";

$rowcount = int(shift(@_) || 1000);

$tempdir = tempdir("/tmp/zolden-test-$testname.XXXXXX", CLEANUP => 1);

$csvfilename_unsorted = "$tempdir/data1.csv";
$csvfilename_sorted = "$tempdir/data2.csv";
$csvfilename_got = "$tempdir/data3.csv";
$shfilename_insert = "$tempdir/insert.sh";
open $csvfile, ">", $csvfilename_unsorted;
open $shfile, ">", $shfilename_insert;

$cb = sub {
	print $csvfile join(",", @_), "\n";
	$teacher = shift @_;
	$subject = shift @_;
	$room = shift @_;
	$group = shift @_;
	$day = shift @_;
	$lesson = shift @_;

	$query = "insert teacher = \"$teacher\", subject = \"$subject\", room = $room, group = $group, day = $day, lesson = $lesson;";
	print $shfile "echo '$query' | ./build/zol &\n";
};

print STDERR "Generating the table\n";
print $shfile "#!/bin/bash\n";
print $shfile "\n";
print $shfile "\n";
maketable( \$cb, $rowcount, 100, 1000);
print $shfile "\n";
print $shfile "wait\n";

close $csvfile;
close $shfile;
chmod 0700, $shfilename_insert;

print STDERR "Sorting\n";
system("sort -u $csvfilename_unsorted > $csvfilename_sorted");
unlink $csvfilename_unsorted;

print STDERR "Cleaning the DB\n";
system("echo 'remove;' | ./build/zol >&/dev/null");

print STDERR "Inserting\n";
system("$shfilename_insert >&/dev/null");

print STDERR "Selecting\n";
system("echo 'print;' | ./build/zol | ./test/print2csv.pl | sort -u > $csvfilename_got");

print STDERR "Cleaning the DB\n";
system("echo 'remove;' | ./build/zol >&/dev/null");

print STDERR "Comparing\n";
if(system("diff $csvfilename_sorted $csvfilename_got") == 0) {
	print "$pass $testname: $rowcount rows\n";
	exit 0;
} else {
	print "$fail $testname: Database engine error\n";
	exit 1;
}
