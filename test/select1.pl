#!/usr/bin/perl

use 5.012;	# so keys/values/each work on arrays
use strict;
use strict qw(vars refs);
use vars;
use warnings;
use autodie;

push @INC, "./test/";
require testlib;
require randtable;

my $restart = 0;
if($ARGV[0] eq 'restart') {
	$restart = 1;
	shift @ARGV;
}
my $row_count = int(shift(@ARGV) || 100);

running("select1");
find_zolden();
my $tempdir = open_temp_dir();
my $datadir = "$tempdir/data";
mkdir $datadir;
my $zol = start_server($datadir);

our $shfile_insert;
our $shfile_select;
my $shfilename_insert = "$tempdir/insert.sh";
my $shfilename_select = "$tempdir/select.sh";

my $query_begin = "$zol <<-'QUERY_END' &\n";
my $query_end = "QUERY_END\n";

sub my_add_row {
	my $ptable = shift @_;
	my $key = shift @_;
	my $row = shift @_;
	my $arr = $ptable->{$key};
	if(not $arr) {
		$arr = $ptable->{$key} = [ ];
	}
	push @$arr, $row;
};

sub print_select_test {
	my $ptable = shift @_;
	my $key_names = shift @_;
	my $key_count = scalar(@$key_names);
	while(my ($keys, $rows) = each %$ptable)
	{
		my @key_values = split /\Q$;\E/, $keys;
		my @key;
		if($key_count != scalar(@key_values)) {
			die "Invalid key type";
		}
		for(my $i = 0; $i != $key_count; ++$i) {
			$key[$i] = "$key_names->[$i] = \"$key_values[$i]\"";
		}
		my $key = join ", ", @key;
		print $shfile_select "{ sort -u | diff -q <($zol <<<'select $key; print;' | ./test/print2csv.pl | sort) -; } <<-'DATA' &";
		foreach(@$rows) {
			print $shfile_select "\n\t", join ',', @$_;
		}
		print $shfile_select "\nDATA\n\n";
	}
}

my %rows_t;
my %rows_s;
my %rows_r;
my %rows_g;
my %rows_l;

my $cb = sub {
	my @tmp = @_;
	my $row = [ @tmp ];
	my $teacher = shift @_;
	my $subject = shift @_;
	my $room = shift @_;
	my $group = shift @_;
	my $day = shift @_;
	my $lesson = shift @_;

	my $query = "insert teacher = \"$teacher\", subject = \"$subject\", room = $room, group = $group, day = $day, lesson = $lesson;";
	print $shfile_insert "\t$query\n";
	my_add_row(\%rows_t, $teacher, $row);
	my_add_row(\%rows_s, $subject, $row);
	my_add_row(\%rows_r, $room, $row);
	my_add_row(\%rows_g, $group, $row);
	my_add_row(\%rows_l, "$day$;$lesson", $row);
};

print STDERR "Generating the table\n";
open $shfile_insert, ">", $shfilename_insert;
print $shfile_insert "#!/bin/bash\n";
print $shfile_insert "\n";
print $shfile_insert $query_begin;
maketable( \$cb, $row_count);
print $shfile_insert $query_end;
print $shfile_insert "\n";
print $shfile_insert "wait\n";
close $shfile_insert;
chmod 0700, $shfilename_insert;

print STDERR "Inserting\n";
system "$shfilename_insert";

if($restart) {
	print STDERR "Restarting\n";
	stop_server();
	$zol = start_server($datadir);
}

print STDERR "Reprinting the table\n";
open $shfile_select, ">", $shfilename_select;
print $shfile_select "#!/bin/bash\n";
print $shfile_select "\n";
print_select_test(\%rows_t, [ "teacher" ]);
print_select_test(\%rows_s, [ "subject" ]);
print_select_test(\%rows_r, [ "room" ]);
print_select_test(\%rows_g, [ "group" ]);
print_select_test(\%rows_l, [ "day", "lesson" ]);
print $shfile_select "\n";
print $shfile_select <<'CODE'
ec=0;
while [ -n "$(jobs)" ]; do
	if ! wait -n; then
		ec=1;
		break;
	fi;
done;
wait;
exit $ec;
CODE
;
close $shfile_select;
chmod 0700, $shfilename_select;

print STDERR "Selecting\n";
my $result = system "$shfilename_select";

stop_server();

if($result == 0) {
	print STDERR "Test passed\n";
	pass("$row_count rows\n");
	close_temp_dir();
} else {
	print STDERR "Test failed\n";
	fail("Database engine error (or external influence)\n");
}
