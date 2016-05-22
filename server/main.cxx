#include <atomic>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include "args.hxx"
#include "db/db.hxx"
#include "client.hxx"
#include "config.hxx"
#include "misc.hxx"
#include "net.hxx"
#include "rtcheck.hxx"

std::atomic<bool> running(true);
std::unique_ptr<Database> db;
File server;
AddressIPv4 address;
pthread_t main_thread;

void mySignalHandler(int signal)
{
	std::clog << "Signal caught: " + std::to_string(signal) << std::endl;
	if(!pthread_equal(pthread_self(), main_thread))
	{
		std::clog << "Redirecting signal to the main thread" << std::endl;
		pthread_sigqueue(main_thread, signal, {0});
		return;
	}
	switch(signal)
	{
		case SIGINT:
			std::cout << "INT" << std::endl;
			running = false;
			break;
		case SIGHUP:
			std::cout << "HUP" << std::endl;
			break;
	}
}

void setupSignalHandling()
{
	struct sigaction act;
	std::memset(&act, 0, sizeof(act));
	act.sa_handler = mySignalHandler;
	sigaction(SIGPIPE, &act, nullptr);
	sigaction(SIGINT, &act, nullptr);
}

void daemonize()
{
	pid_t child = fork();
	if(child == 0)
		return;
	syserror_throwif(child < 0, "Can't go to background");
	std::cout << child << std::endl; // Return PID
	std::exit(0);
}

int main(int argc, char **argv)
{
	main_thread = pthread_self();
	setupSignalHandling();
	arguments::read(argc, argv);
	std::string txtfile = arguments::get("preload");
	std::string datadir = arguments::get("data");
	std::string addr = arguments::get("addr", zolden_addr);
	std::string port = arguments::get("port", std::to_string(zolden_port));
	std::clog << "Zolden database manipulation program" << std::endl;
	if(datadir.empty())
	{
		datadir = "/tmp";
		std::clog << "Warning: data directory is not specified. Using invisible temporary files in " << datadir << std::endl;
		db.reset(new Database(datadir, db_temporary));
	}
	else
		db.reset(new Database(datadir));
	if(!txtfile.empty())
	{
		std::clog << "Reading database from: " << txtfile << std::endl;
		db->readText(txtfile);
	}
	server = Bind(addr, port, true);
	syserror_throwif(listen(server.get(), 20), "Can't listen on a socket");
	address = GetSocketAddressIPv4(server);
	std::clog << "Listening at " << std::to_string(address) << std::endl;
	if(arguments::present("daemon"))
	{
		std::clog << "Going into background..." << std::endl;
		daemonize();
		std::clog << "Daemonized" << std::endl;
	}
	else
	{
		std::clog << "Remaining in foreground" << std::endl;
		std::cout << "READY " << address.port << std::endl;
	}
	while(running)
	{
		File fd(accept(server.get(), nullptr, nullptr));
		if(!fd)
		{
			if(errno == EINTR)
				continue;
			syserror("Can't accept new connection");
		}
		std::string id = std::to_string(fd.get());
		std::cerr << "[MAIN] Client connected. (" + id + ")\n";
		std::thread t(Client(std::move(fd)));
		t.detach();
		std::cerr << "[MAIN] Thread started. (" + id + ")\n";
	}
	std::clog << "Shutting down" << std::endl;
	std::cout << "SHUTDOWN" << std::endl;
	pthread_exit(nullptr);
}
