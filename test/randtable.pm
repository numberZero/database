#!/usr/bin/perl

use vars;
use warnings;

my @chars = split "", "abcdefghijklmnopqrstuvwxyz";
my $chars_count = scalar(@chars);

sub getvalue {
	my $table = shift(@_);
	my $count = scalar(@{$table});
	my $index = int(rand($count));
	return $table->[$index];
}

sub mkstring {
	my $length = shift(@_);
	my $result = "";
	for(my $i = 0; $i != $length; ++$i) {
		my $k = int(rand($chars_count));
		$result .= $chars[$k];
	}
	return $result;
}

sub maketable {
	local *callback = shift(@_);
	my $row_count = shift(@_);
	my $s_count = shift(@_) || 100;
	my $t_count = shift(@_) || 1000;

	for($i = 0; $i != $s_count; ++$i) {
		$subjects[$i] = uc mkstring int(10 + rand(20));
	}

	for($i = 0; $i != $t_count; ++$i) {
		$teachers[$i] = mkstring int(10 + rand(20));
		$ts[$i] = int(rand($s_count));
	}

	for($i = 0; $i != $row_count; ++$i) {
		$tid = int(rand($t_count));
		$teacher = $teachers[$tid];
		$subject = $subjects[$ts[$tid]];
		$room = 100 + int(rand(900));
		$group = 100 + int(rand(600));
		$day = 1 + int(rand(6));
		$lesson = 1 + int(rand(6));
		&$callback($teacher, $subject, $room, $group, $day, $lesson);
	}
}

return TRUE;
