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

$row_count = int(shift(@ARGV) || 1000);
$group_size = int(shift(@ARGV) || ($row_count / 100));

sub tryclient {
	my $filename = shift @_;
	(-x $filename) and $filename;
}

$tempdir = tempdir("/tmp/zolden-test-$testname.XXXXXX");
$client = tryclient("./release/zol") || tryclient("./build/zol") || tryclient("./zol") || die "Can't find Zolden client";

$csvfilename_unsorted = "$tempdir/data1.csv";
$csvfilename_sorted = "$tempdir/data2.csv";
$csvfilename_got = "$tempdir/data3.csv";
$shfilename_insert = "$tempdir/insert.sh";
open $csvfile, ">", $csvfilename_unsorted;
open $shfile, ">", $shfilename_insert;

$ingroup_count = 0;
$query_begin = "echo";
$query_end = " | $client &\n";

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
system("sort -u $csvfilename_unsorted > $csvfilename_sorted");

print STDERR "Cleaning the DB\n";
system("echo 'remove;' | $client >&/dev/null");

print STDERR "Inserting\n";
system("$shfilename_insert >/dev/null");

print STDERR "Selecting\n";
system("echo 'print;' | $client | ./test/print2csv.pl | sort -u > $csvfilename_got");

print STDERR "Comparing\n";
if(system("diff $csvfilename_sorted $csvfilename_got") == 0) {
	print STDERR "pass\n";

	print STDERR "Cleaning the DB\n";
	system("echo 'remove;' | $client >&/dev/null");

	print "$pass $testname: $row_count rows in groups of $group_size rows each\n";
	system("rm -r $tempdir");
	exit 0;
} else {
	print STDERR "fail\n";
	print STDERR "Keeping the DB dirty\n";
	print "$fail $testname: Database engine error; see $tempdir\n";
	exit 1;
}
