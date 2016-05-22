#!/usr/bin/perl

sub readitem { # reads into global %row
	my $name = shift(@_);
	my $sep = shift(@_) || ",";
	$_ = <> or die "Can't read from the file: $!\n";
	m/^$name: (.+)$/ or die "Unexpected item: $_\n";
	$row{$name} = $1;
	print "$1$sep";
}

while(<>) {
	if(m/\*\*\* Row (\d+) \*\*\*\n$/) { # row start
		%row = ();
		readitem "teacher";
		readitem "subject";
		readitem "room";
		readitem "group";
		readitem "day";
		readitem "lesson", "\n";
		<> eq "\n" or die "Empty line expected\n";
	}
	elsif(m/^\*\*\* END \*\*\*\n$/) {
		break;
	}
}
