use vars;
use warnings;
use File::Temp qw(tempdir);
use File::Path qw(remove_tree);
 
our $gray = "\e[m";
our $red = "\e[31m";
our $green = "\e[32m";

my @PATH = ("./bin", "./release", "./build", "./", "/usr/bin", "/bin");

my $testname;
my $client;
my $server;
my $port;
my $pid;

sub findexec {
	my $name = shift @_;
	for my $dir (@PATH) {
		my $filename = "$dir/$name";
		print STDERR "Trying to find $name in $dir: checking $filename\n";
		if(-x $filename) {
			return $filename;
		}
	}
}

sub running {
	$testname = shift @_;
}

# Finds Zolden executables
# Stores paths in $client and $server
sub find_zolden {
	$client = findexec("zol") or die "Can't find Zolden client";
	$server = findexec("zold") or die "Can't find Zolden server";
}

# Initializes temporary directory
# Path is stored in global $tempdir
sub open_temp_dir {
	defined $tempdir and die "Temporary directory is already open";
	$tempdir = tempdir("/tmp/zolden-test-$testname.XXXXXX");
}

sub close_temp_dir {
	defined $tempdir or die "Temporary directory is not open";
	remove_tree($tempdir);
	undef $tempdir;
}

# Starts Zolden server
# Reguires $client and $server to be set
# Sets $zol to client command line
sub start_server {
	defined $server or die "Server has not been found";
	defined $pid and die "Server already started";
	$port = int(30000 + rand(30000));

	my $tmppid = open(CHILD, "-|");
	defined $tmppid or die "Can't fork: $!";
	if (not $tmppid) {
		exec $server, '--daemon', '--port', $port or die "Can't exec: $!";
	}
	my $line = <CHILD>;
	$pid = int($line) or die "Can't start the server: $line";
	close CHILD;
	defined $client and $zol = "$client 127.0.0.1 $port";
}

# Stops Zolden server
sub stop_server {
	kill 'TERM', $pid;
	undef $pid;
}

sub pass {
	print "[${green}PASS${gray}] ", @_;
}

sub fail {
	print "[${red}FAIL${gray}] ", @_;
}

END {
	defined $pid and print "Server still active; port $port, pid $pid\n";
	defined $tempdir and print "Temporary directory still exists at $tempdir\n";
}

return TRUE;
