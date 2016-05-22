use vars;
use warnings;
use File::Temp qw(tempdir);
use File::Path qw(remove_tree);
 
my @PATH = ("./bin", "./release", "./build", "./", "/usr/bin", "/bin");

my $testname;
my $client;
my $server;
my $port;
my $pid;
my $result;

my $tempdir;
my $zol;

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
	return $tempdir;
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
	my $datadir = shift @_;
	my @args = ($server, '--port', $port);
	if($datadir) {
		push @args, '--data', $datadir;
	}
	$port = 0;
	print STDERR "Starting server at port $port\n";
	$pid = open CHILD, '-|', @args or die "Can't start server: $!";
	$_ = <CHILD>;
	m/^READY ([0-9]+)/ or die "Server refused to start: $_";
	$port = int($1);
	defined $client and $zol = "$client --addr 127.0.0.1 --port $port --batch";
	print STDERR "Server started at port $port, PID $pid\n";
	return $zol;
}

# Stops Zolden server
sub stop_server {
	print STDERR "Stopping server\n";
	kill 'TERM', $pid;
	while(<CHILD>) {
		last if m/^SHUTDOWN/;
	}
	print STDERR "Shutdown request accepted\n";
	waitpid($pid, 0) == $pid or die "Server is not running..?";
	my $result = $? >> 8;
	close CHILD;
	undef $pid;
	if($result == 0) {
		print STDERR "Server stopped\n";
	} else {
		crash("Server crashed with error code $?");
	}
}

sub pass {
	$result = 0;
	print "[\e[32mPASS\e[m] ", @_;
}

sub fail {
	$result = 1;
	print "[\e[31mFAIL\e[m] ", @_;
}

sub crash {
	$result = -1;
	print "[\e[31mXCPT\e[m] ", @_;
	exit $result;
}

END {
	defined $pid and print STDERR "Server may still be active; port $port, pid $pid\n";
	defined $tempdir and print STDERR "Temporary directory may still exist at $tempdir\n";
	if(not defined $result) {
		crash("No result given");
	}
	exit $result;
}

return TRUE;
